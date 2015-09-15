#include <I2Cdev.h>
#include <Servo.h>
#include "Pins.h"
#include "Blinker.h"
#include "Mpu9150.h"
#include "Car.h"

#define count_of(a) (sizeof(a)/sizeof(a[0]))

#include "Ping.h"

#define PLAY_SOUNDS 0

bool TRACE_RX = false;
bool TRACE_PINGS = false;
bool TRACE_ESC = false;
bool TRACE_MPU = false;
bool TRACE_LOOP_SPEED = false;
bool TRACE_DYNAMICS = true;


// computer steering and speed
class PwmInput {
public:
  unsigned long last_trigger_us = 0;
  unsigned long pulse_width_us = 0;

  // since we are dealing with standard RC,
  // anything out of of the below ranges should not occur and is ignored
  unsigned long max_pulse_us = 2000;
  unsigned long min_pulse_us = 1000;

  unsigned long last_pulse_ms = 0;  // time when last pulse occurred


  // milliseconds without a pulse to consider a timeout
  unsigned long timeout_ms = 500;
  int pin;

  void attach(int pin) {
    this->pin = pin;
    last_trigger_us = 0;
    pulse_width_us = 0;
    pinMode(pin, INPUT);
  }

  // interrupt handler
  inline void handle_change() {
    unsigned long us = micros();
    if(digitalRead(pin)) {
      last_trigger_us = us;
    }
    else {
      unsigned long width = us - last_trigger_us;
      // only accept pulses in acceptable range
      if(width >= min_pulse_us && width <= max_pulse_us) {
        pulse_width_us = width;
        last_pulse_ms = millis();
      }
    }
  }

  bool is_valid() {
    return millis() - last_pulse_ms < timeout_ms;
  }

  // safe method to return pulse width in microseconds
  // returns 0 if invalid
  int pulse_us() {
    if(is_valid())
      return pulse_width_us;
    else
      return 0;
  }



  void trace() {
    Serial.print(pin);
    Serial.print(" ");
    Serial.print(pulse_width_us);
    if(is_valid())
      Serial.print(" valid");
    else
      Serial.print(" invalid");
  }
};


class Beeper {
public:
  int pin;

  // hz of notes
  const int note_c5 = 523;
  const int note_d5 = 587;
  const int note_e5 = 659;
  const int note_f5 = 698;
  const int note_g5 = 784;
  const int note_a5 = 880;
  const int note_b5 = 988;

  // duration of notes
  const unsigned long note_ms = 500;
  const unsigned long gap_ms = 10;

  void attach(int pin) {
    this->pin = pin;
  }

  void play(int note) {
    tone(pin,note);
  }

  void beep(int note) {
#if(PLAY_SOUNDS)
    tone(pin, note, note_ms);
    delay(note_ms + gap_ms);
#endif
  }

  void beep_ascending() {
    beep(note_c5);
    beep(note_e5);
    beep(note_g5);
  }

  void beep_descending() {
    beep(note_g5);
    beep(note_e5);
    beep(note_c5);
  }

  void beep_nbc() {
    beep(note_c5);
    beep(note_a5);
    beep(note_f5);
  }

  void beep_warble() {
    beep(note_a5);
    beep(note_f5);
    beep(note_a5);
    beep(note_f5);
    beep(note_a5);
    beep(note_f5);
    beep(note_a5);
    beep(note_f5);
  }
};

/*
enum rx_event {
  uninitialized = 0;
  steer_unknown = 1;
  steer_left = 2;
  steer_center = 4;
  steer_right = 8;
  power_unknown = 16;
  power_reverse = 32;
  power_neutral = 64;
  power_forward = 128;

}
*/



class EventQueue {
public:
  static const int size = 10;

  RxEvent events[size];

  void add(RxEvent new_event) {
    for(int i = size-1; i > 0; --i) {
      events[i]=events[i-1];
    }
    events[0] = new_event;

    if(TRACE_RX) {
      for(int i = 0; i < size; i++) {
        Serial.print(events[i].steer);
        Serial.print(events[i].speed);
        Serial.print(",");
      }
      Serial.println();
    }
  }

  bool matches(const RxEvent * pattern, int count) {
    for(int i = 0; i < count; ++i) {
      if(!events[i].equals(pattern[count-1 - i])) {
        return false;
      }
    }
    return true;
  }
};


class RxEvents {
public:
  RxEvent current, pending;
  bool new_event = false;
  EventQueue recent;

  // number of different readings before a new event is triggered
  // this is used to de-bounce the system
  const int change_count_threshold = 5;
  int change_count = 0;

  void process_pulses(int steer_us, int speed_us) {
    pending.steer = steer_code(steer_us);
    pending.speed = speed_code(speed_us);
    if (! pending.equals(current)) {
      if(++change_count >= change_count_threshold) {
        current = pending;
        change_count = 0;
        recent.add(current);
        new_event = true;
      }
    }
  }


   char steer_code(int steer_us) {
    if (steer_us == 0)
      return '?';
    if (steer_us < 1300)
      return 'R';
    if (steer_us > 1700)
      return 'L';
    return 'C';
  }

  char speed_code(int speed_us) {
    if (speed_us == 0)
      return '?';
    if (speed_us < 1300)
      return 'F';
    if (speed_us > 1700)
      return 'V';
    return 'N';
  }

  // returns true if new event received since last call
  bool get_event() {
    bool rv = new_event;
    new_event = false;
    return rv;
  }

  void trace() {
    Serial.write(current.steer);
    Serial.write(current.speed);
  }
};




class Esc {
public:

  Servo * speed;

  const unsigned long brake_ms = 500;
  const unsigned long pause_ms = 200;
  unsigned long brake_start_ms = 0;
  unsigned long pause_start_ms = 0;

  const int forward_us =  1390;
  const int reverse_us =  1610;


  const int neutral_us = 1500;
  int calibration_us = 0;
  int current_us = -1;

  eSpeedCommand command = speed_neutral;

  enum eState {
    stopped,
    forward_braking,
    reverse_braking,
    forward,
    reverse,
    pausing
  } state;

  const char * state_name(eState s) {
    const char *names[]  = {
      "stopped",
      "forward_braking",
      "reverse_braking",
      "forward",
      "reverse",
      "pausing"
    };
    return names[s];
  }

  // used to match the control stick settings.
  // Will use setting as the new pulse width for
  // neutral
  void set_neutral_pwm_us(unsigned long us) {
    calibration_us = us - neutral_us;
  }

  void init(Servo * speed) {
    this->speed = speed;
    state = stopped;
    set_pwm_us(neutral_us);
  }

  // sets pulse width, adjusted by calibration if any
  void set_pwm_us(int us) {
    int c_us = us + calibration_us;
    if(c_us != current_us) {
      current_us = c_us;
      speed->writeMicroseconds(current_us);
      Serial.print("ESC set pulse: ");
      Serial.println(current_us);
    }
  }

  void set_command(eSpeedCommand new_command) {
    if(command == new_command) {
      return;
    }

    command = new_command;
    Serial.print("ESC Command: ");
    Serial.println(speed_command_name(command));
    Serial.print("OLD ESC State: ");
    Serial.println(state_name(state));

    if(command == speed_forward) {
      switch(state) {
        case forward_braking:
        case stopped:
        case forward:
          state = forward;
          set_pwm_us(forward_us);
          break;

        case pausing:
          break;

        case reverse_braking:
          set_pwm_us(forward_us);
          break;

        case reverse:
          set_pwm_us(forward_us);
          state = reverse_braking;
          brake_start_ms = millis();
          break;
      }
    }
    if(command == speed_reverse) {
      switch(state) {
        case reverse_braking:
        case stopped:
        case reverse:
          state = reverse;
          set_pwm_us(reverse_us);
          break;

        case pausing:
          break;

        case forward_braking:
          set_pwm_us(reverse_us);
          break;

        case forward:
          state = forward_braking;
          set_pwm_us(reverse_us);
          brake_start_ms = millis();
          break;
      }
    }
    if(command == speed_neutral){
      switch(state) {
        case stopped:
        case pausing:
          set_pwm_us(neutral_us);
          break;

        case forward_braking:
          set_pwm_us(reverse_us);
          break;

        case reverse_braking:
          set_pwm_us(forward_us);
          break;

        case reverse:
          state = reverse_braking;
          set_pwm_us(forward_us);
          brake_start_ms = millis();
          break;

        case forward:
          state = forward_braking;
          set_pwm_us(reverse_us);
          brake_start_ms = millis();
          break;
      }
    }
    Serial.print("New ESC State: ");
    Serial.println(state_name(state));
  }

  void execute() {
    if(state == forward_braking || state == reverse_braking) {
      unsigned long ms = millis();
      if(ms - brake_start_ms >= brake_ms) {
        Serial.println("Braking complete");
        state = pausing;
        pause_start_ms = ms;
        set_pwm_us(neutral_us);
      }
    }

    if(state == pausing) {
      unsigned long ms = millis();
      if(ms - pause_start_ms >= pause_ms) {
        Serial.println("Pausing complete");
        state = stopped;
      }
    }

    if(state == stopped) {
      if(command == speed_forward) {
        Serial.println("forward from stopped");
        set_pwm_us(forward_us);
        state = forward;
      }
      if(command == speed_reverse) {
        Serial.println("reverse from stopped");
        set_pwm_us(reverse_us);
        state = reverse;
      }
      if(command == speed_neutral) {
        set_pwm_us(neutral_us);
        state = stopped;
      }
    }
  }
};


class CommandInterpreter{
public:
  String buffer;
  const Command * commands;
  int command_count;

  void init(const Command * _commands, int _command_count)
  {
    commands = _commands;
    command_count = _command_count;
  }

  void execute() {
    while(Serial.available()>0) {
      char c = Serial.read();
      if( c == '\n') {
        process_command(buffer);
        buffer = "";
      } else {
        buffer += c;
      }
    }
  }

  void process_command(String s) {
    for(int i = 0; i < command_count; i++) {
      if(s.equals(commands[i].name)) {
        Serial.print("Executing command ");
        Serial.println(commands[i].name);
        commands[i].f();
        return;
      }
    }
    Serial.print("Unknown command: ");
    Serial.println(s);
  }
};


//////////////////////////
// Globals

Servo steering;
Servo speed;
Esc esc;

PwmInput rx_steer;
PwmInput rx_speed;
RxEvents rx_events;
Ping ping;
Beeper beeper;
Blinker blinker;
CommandInterpreter interpreter;


// diagnostics for reporting loop speeds
unsigned long loop_count = 0;
unsigned long loop_time_ms = 0;
unsigned long last_loop_time_ms = 0;
unsigned long last_report_ms = 0;
unsigned long last_report_loop_count = 0;


enum {
  mode_manual,
  mode_automatic,
  mode_circle
} mode;


///////////////////////////////////////////////
// Interrupt handlers

void rx_str_handler() {
  rx_steer.handle_change();
}

void rx_spd_handler() {
  rx_speed.handle_change();
}


eSpeedCommand speed_for_ping_inches(double inches) {
  if(inches ==0) return speed_neutral;
  // get closer if far
  if (inches > 30.)
    return speed_forward;
  // back up if too close
  if (inches < 20.)
    return speed_reverse;

  return speed_neutral;
}

class CircleMode {
public:
  double last_angle;
  double degrees_turned = 0;
  Mpu9150 * mpu;
  bool done = false;

  bool is_done() {
    return done;
  }

  void init(Mpu9150 * _mpu) {
    mpu = _mpu;
    last_angle = mpu->ground_angle();
    degrees_turned = 0;
    done = false;
  }

  void end() {
    esc.set_command(speed_neutral);
    steering.writeMicroseconds(1500); // look straight ahead
  }

  void execute() {
//    Serial.print("circle has turned");
//    Serial.println(degrees_turned);
    double ground_angle = mpu->ground_angle();
    double angle_diff = last_angle-ground_angle;
    if(abs(angle_diff) > 70){
      last_angle = ground_angle; // cheating low tech way to avoid wrap around
      return;
    }
    degrees_turned += angle_diff;
    last_angle = ground_angle;
    if(abs(degrees_turned) < 90) {
      steering.writeMicroseconds(1900); // turn left todo: make steer commands
      esc.set_command(speed_forward);
    } else {
      esc.set_command(speed_neutral);
      steering.writeMicroseconds(1500); // look straight ahead
      done = true;
      Serial.println("circle complete");
    }
  }
};


const RxEvent auto_pattern [] =
  {{'R','N'},{'C','N'},{'R','N'},{'C','N'},{'R','N'},{'C','N'},{'R','N'}};
const RxEvent circle_pattern [] =
  {{'L','N'},{'C','N'},{'L','N'},{'C','N'},{'L','N'},{'C','N'},{'L','N'}};



void trace_ping_on() {
  TRACE_PINGS = true;
}

void trace_ping_off() {
  TRACE_PINGS = false;
}

void trace_esc_on() {
  TRACE_ESC = true;
}

void trace_esc_off() {
  TRACE_ESC = false;
}

void trace_mpu_on() {
  TRACE_MPU = true;
}

void trace_mpu_off() {
  TRACE_MPU = false;
}

void trace_loop_speed_on() {
  TRACE_LOOP_SPEED = true;
}

void trace_loop_speed_off() {
  TRACE_LOOP_SPEED = false;
}


void trace_dynamics_on() {
  TRACE_DYNAMICS = true;
}

void trace_dynamics_off() {
  TRACE_DYNAMICS = false;
}


void help();



const Command commands[] = {
  {"help", help},
  {"trace dynamics on", trace_dynamics_on},
  {"trace dynamics off", trace_dynamics_off},
  {"trace ping on", trace_ping_on},
  {"trace ping off", trace_ping_off},
  {"trace esc on", trace_esc_on},
  {"trace esc off", trace_esc_off},
  {"trace mpu on", trace_mpu_on},
  {"trace mpu off", trace_mpu_off},
  {"trace loop speed on", trace_loop_speed_on},
  {"trace loop speed off", trace_loop_speed_off}
};

void help() {
  for(unsigned int i = 0; i < count_of(commands); i++)
    Serial.println(commands[i].name);
}


Mpu9150 mpu9150;

void setup() {
  Serial.begin(9600);
  delay(1000);
  interpreter.init(commands,count_of(commands));
  Serial.println("setup begun");
  steering.attach(PIN_U_STEER);
  steering.writeMicroseconds(1500);
  speed.attach(PIN_U_SPEED);
  speed.writeMicroseconds(1500);


  rx_steer.attach(PIN_RX_STEER);
  rx_speed.attach(PIN_RX_SPEED);
  esc.init(&speed);
  blinker.init();

  ping.init(PIN_PING_TRIG, PIN_PING_ECHO);

  int int_str = PIN_RX_STEER;
  int int_esc = PIN_RX_SPEED;

  attachInterrupt(int_str, rx_str_handler, CHANGE);
  attachInterrupt(int_esc, rx_spd_handler, CHANGE);


  pinMode(PIN_PING_TRIG, OUTPUT);
  pinMode(PIN_PING_ECHO, INPUT);

  digitalWrite(PIN_PING_TRIG, LOW);

  mode = mode_manual;

  beeper.attach(PIN_SPEAKER);
  beeper.beep_nbc();

  last_report_ms = millis();

  Serial.println("car_control begun");
  mpu9150.setup();
  delay(1000);
  mpu9150.zero();
}

CircleMode circle_mode;

// returns true if loop time passes through n ms boundary
bool every_n_ms(unsigned long last_loop_ms, unsigned long loop_ms, unsigned long ms) {
  return (last_loop_ms % ms) + (loop_ms - last_loop_ms) >= ms;

}

void loop() {
  // set global loop values
  loop_count++;
  last_loop_time_ms = loop_time_ms;
  loop_time_ms = millis();

  // get common execution times
  bool every_second = every_n_ms(last_loop_time_ms, loop_time_ms, 1000);
  bool every_100_ms = every_n_ms(last_loop_time_ms, loop_time_ms, 100);

  // get commands from usb
  interpreter.execute();


  ping.execute();
  mpu9150.execute();
  blinker.execute();

  rx_events.process_pulses(rx_steer.pulse_us(), rx_speed.pulse_us());

  bool new_rx_event = rx_events.get_event();

  double inches = ping.inches();//ping_inches();
  bool new_ping = ping.new_data_ready();
  if(TRACE_PINGS && new_ping) {
    Serial.print("ping inches:");
    Serial.println(inches);
  }

  if(0) {
    Serial.print("Speed: ");
    Serial.print(rx_speed.pulse_us());
    Serial.print("Steer: ");
    Serial.print(rx_steer.pulse_us());
    Serial.println();
  }
  switch (mode) {
    case mode_manual:
      if(rx_steer.pulse_us() > 0 && rx_speed.pulse_us() > 0) {
        steering.writeMicroseconds(rx_steer.pulse_us());
        speed.writeMicroseconds(rx_speed.pulse_us());
      } else {
        steering.writeMicroseconds(1500);
        speed.writeMicroseconds(1500);
      }

      if (new_rx_event && rx_events.recent.matches(auto_pattern, count_of(auto_pattern))) {
        mode = mode_automatic;
        esc.set_command(speed_neutral);
        beeper.beep_ascending();
        Serial.print("switched to automatic");
      }
      if (new_rx_event && rx_events.recent.matches(circle_pattern, count_of(circle_pattern))) {
        esc.set_command(speed_neutral);
        mode = mode_circle;
        circle_mode.init(&mpu9150);
        beeper.beep_ascending();
        Serial.print("switched to circle");
      }

      break;

    case mode_automatic:

      steering.writeMicroseconds(1500);
      esc.set_command(speed_for_ping_inches(inches));
      esc.execute();
      if (new_rx_event && rx_events.current.equals(RxEvent('L','N'))) {
        mode = mode_manual;
        esc.set_command(speed_neutral);
        steering.writeMicroseconds(1500);
        speed.writeMicroseconds(1500);
        beeper.beep_descending();
        Serial.println("switched to manual - user initiated");
      }
      if (new_rx_event && rx_events.current.is_bad()) {
        mode = mode_manual;
        esc.set_command(speed_neutral);
        steering.writeMicroseconds(1500);
        speed.writeMicroseconds(1500);
        beeper.beep_warble();
        Serial.println("switched to manual - no coms");
      }
      break;

    case mode_circle:
      circle_mode.execute();
      esc.execute();
      // todo: merge this code into mode manager`
      if (circle_mode.is_done()) {
        mode = mode_manual;
        esc.set_command(speed_neutral);
        steering.writeMicroseconds(1500);
        speed.writeMicroseconds(1500);
        beeper.beep_descending();
        Serial.println("switched to manual - circle complete");
      }
      if (new_rx_event && rx_events.current.equals(RxEvent('R','N'))) {
        mode = mode_manual;
        esc.set_command(speed_neutral);
        steering.writeMicroseconds(1500);
        speed.writeMicroseconds(1500);
        beeper.beep_descending();
        Serial.println("switched to manual - user initiated");
      }
      if (new_rx_event && rx_events.current.is_bad()) {
        mode = mode_manual;
        esc.set_command(speed_neutral);
        steering.writeMicroseconds(1500);
        speed.writeMicroseconds(1500);
        beeper.beep_warble();
        Serial.println("switched to manual - no coms");
      }
      break;
  }



  // state tracing
  if (0) {
    rx_steer.trace();
    Serial.print(",");
    rx_speed.trace();
    Serial.println();
  }
  if(every_100_ms && TRACE_DYNAMICS) {
    Serial.print("str");
    Serial.print(", ");
    Serial.print(steering.readMicroseconds());
    Serial.print(", ");
    Serial.print("esc");
    Serial.print(", ");
    Serial.print(speed.readMicroseconds());
    Serial.print(", ");
    Serial.print("aa");
    Serial.print(", ");
    Serial.print(mpu9150.aa.x - mpu9150.a0.x);
    Serial.print(", ");
    Serial.print(mpu9150.aa.y  - mpu9150.a0.y);
    Serial.print(", ");
    Serial.print(mpu9150.aa.z  - mpu9150.a0.z);
    Serial.print(", ");
    Serial.print("angle");
    Serial.print(", ");
    Serial.print(mpu9150.ground_angle());
    Serial.println();
  }

  if(every_second && TRACE_MPU) {
      mpu9150.trace_status();
  }

  // loop speed reporting
  //  12,300 loops per second as of 8/18 9:30
  // 153,400 loops per second on Teensy 3.1 @ 96MHz 8/29
  //  60,900 loops per second on Teensy 3.1 @ 24MHz 8/29
  if(every_second && TRACE_LOOP_SPEED) {
    unsigned long loops_since_report = loop_count - last_report_loop_count;
    double seconds_since_report =  (loop_time_ms - last_report_ms) / 1000.;

    Serial.print("loops per second: ");
    Serial.print( loops_since_report / seconds_since_report );
    Serial.print(" microseconds per loop ");
    Serial.print( 1E6 * seconds_since_report / loops_since_report );
    Serial.println();

    // remember stats for next report
    last_report_ms = loop_time_ms;
    last_report_loop_count = loop_count;
  }

}
