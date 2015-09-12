#include "MPU6050_9Axis_MotionApps41.h"
#include <MPU6050.h>
#include <helper_3dmath.h>

#include <Wire.h>

#include <I2Cdev.h>

#include <Servo.h>
#include "mpu9150.h"
#include "car_control.h"

#define count_of(a) (sizeof(a)/sizeof(a[0]))

#define PIN_RX_SPEED 1
#define PIN_RX_STEER 2

#define PIN_U_SPEED 3
#define PIN_U_STEER 4
#define PIN_PING_TRIG 6
#define PIN_PING_ECHO 23

#define PIN_SPEAKER   9
#define PIN_LED 13

#define PLAY_SOUNDS 1

bool TRACE_PINGS = false;
bool TRACE_ESC = false;
bool TRACE_MPU = false;
bool TRACE_LOOP_SPEED = false;

struct Blinker {
  int period_ms = 1000;
  int on_ms = 1;
  int pin = PIN_LED;
  unsigned long wait_ms = 0;
  unsigned long last_change_ms = 0;
  bool is_on = false;

  void init(int _pin = PIN_LED) {
    pin = _pin;
    pinMode(pin, OUTPUT);
  }

  void execute() {
    unsigned long ms = millis();
    if(ms - last_change_ms <= wait_ms)
      return;
    is_on = !is_on;
    last_change_ms = ms;
    digitalWrite(pin, is_on);
    if(is_on) {
      wait_ms =on_ms;
    }
    else {
      wait_ms = period_ms - on_ms;
    }
  }
};

// computer steering and speed
struct PwmInput {
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
  void handle_change() {
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


struct Beeper {
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

struct RxEvent {
  char speed;
  char steer;

  RxEvent():RxEvent('0','0'){
  }

  RxEvent(char steer, char speed) {
    this->steer = steer;
    this->speed = speed;
  }

  bool equals(RxEvent other) {
    return speed == other.speed && steer == other.steer;
  }

  bool is_bad(){
    return speed == '?' || steer == '?';
  }
};


struct EventQueue {
  static const int size = 10;

  RxEvent events[size];

  void add(RxEvent new_event) {
    for(int i = size-1; i > 0; --i) {
      events[i]=events[i-1];
    }
    events[0] = new_event;

    if(0) {
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


struct RxEvents {
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


struct Ping {
  int ping_pin, echo_pin;
  unsigned long ping_start_ms = 0;
  unsigned long ping_start_us = 0;
  unsigned long reply_start_us = 0;
  bool _new_data_ready = false;

  double last_ping_distance_inches = 0.;

  unsigned long ping_rate_ms = 100;
  const unsigned long ping_timeout_us = 20000; // 20000 microseconds should be about 10 feet

  enum {
    no_ping_pending,
    waiting_for_reply_start,
    waiting_for_reply_end
  } state;



  void init(int _ping_pin, int _echo_pin){
    ping_pin = _ping_pin;
    echo_pin = _echo_pin;
    digitalWrite(ping_pin, LOW);
    state = no_ping_pending;
  }

  bool new_data_ready() {
    bool rv = _new_data_ready;
    _new_data_ready = false;
    return rv;
  }

  void set_distance_from_us(int us) {
      double ping_distance_inches = (double) us / 148.; // 148 microseconds for ping round trip per inch
      if(last_ping_distance_inches != ping_distance_inches) {
         last_ping_distance_inches = ping_distance_inches;
         _new_data_ready = true;
      }
  }

  void scan(){
    unsigned long ms = millis();
    unsigned long  us = micros();

    switch(state) {
      case no_ping_pending:
        if( ms - ping_start_ms >= ping_rate_ms) {
          ping_start_ms = ms;
          ping_start_us = us;
          // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
          digitalWrite(ping_pin, HIGH);
          delayMicroseconds(10);  // todo: can we get rid of this delay or maybe it's ok?
          digitalWrite(ping_pin, LOW);
          state = waiting_for_reply_start;
        }
        break;

      case waiting_for_reply_start:
        if(digitalRead(echo_pin) == HIGH) {
          reply_start_us = us;
          state = waiting_for_reply_end;
        }  else if (us - ping_start_us >  ping_timeout_us) {
          set_distance_from_us(0); // zero on timeout
          state = no_ping_pending;
        }
        break;

      case waiting_for_reply_end:
        if(digitalRead(echo_pin) == LOW) {
          set_distance_from_us(us - reply_start_us);
          state = no_ping_pending;
        } else if (us - reply_start_us > ping_timeout_us) {
          set_distance_from_us(0); // zero on timeout
          state = no_ping_pending;
        }

        break;
    }
  }

  inline double inches() {
    return last_ping_distance_inches;
  }
};


struct SpeedControl {

  Servo * speed;

  const unsigned long brake_ms = 200;
  const unsigned long pause_ms = 500;
  unsigned long brake_start_ms = 0;
  unsigned long pause_start_ms = 0;

  const int forward_us =  1200;
  const int reverse_us =  1700;


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
        case stopped:
        case forward:
          state = forward;
          set_pwm_us(forward_us);
          break;

        case pausing:
        case forward_braking:
        case reverse_braking:
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
        case stopped:
        case reverse:
          state = reverse;
          set_pwm_us(reverse_us);
          break;

        case pausing:
        case reverse_braking:
        case forward_braking:
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
        case reverse_braking:
        case forward_braking:
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


typedef void (*voidfunction)();

struct command {
  const char * name;
  voidfunction f;
};


class CommandInterpreter{
public:
  String buffer;
  const command * commands;
  int command_count;

  void init(const command * _commands, int _command_count)
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
SpeedControl speed_control;

PwmInput rx_steer;
PwmInput rx_speed;
RxEvents rx_events;
Ping ping;
Beeper beeper;
Blinker blinker;
CommandInterpreter interpreter;


// diagnostics for reporting loop speeds
unsigned long loop_count = 0;
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
  if (inches > 25.)
    return speed_forward;
  // back up if too close
  if (inches < 12.)
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
    speed_control.set_command(speed_neutral);
    steering.writeMicroseconds(1500); // look straight ahead
  }

  void execute() {
    Serial.print("circle has turned");
    Serial.println(degrees_turned);
    double ground_angle = mpu->ground_angle();
    double angle_diff = last_angle-ground_angle;
    if(abs(angle_diff) > 70){
      last_angle = ground_angle; // cheating low tech way to avoid wrap around
      return;
    }
    degrees_turned += angle_diff;
    last_angle = ground_angle;
    if(abs(degrees_turned) < 360) {
      //speed_control.set_command(speed_forward);
      steering.writeMicroseconds(1900); // turn left todo: make steer commands
    } else {
      speed_control.set_command(speed_neutral);
      steering.writeMicroseconds(1500); // look straight ahead
      Serial.println("circle complete");
    }
  }
};


const RxEvent auto_pattern [] =
  {{'R','N'},{'C','N'},{'R','N'},{'C','N'},{'R','N'},{'C','N'},{'R','N'}};
const RxEvent circle_pattern [] =
  {{'R','N'},{'C','N'},{'R','N'},{'C','N'},{'R','N'},{'C','N'},{'L','N'}};



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

void help();



const command commands[] = {
  {"help", help},
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
  speed.attach(PIN_U_SPEED);


  rx_steer.attach(PIN_RX_STEER);
  rx_speed.attach(PIN_RX_SPEED);
  speed_control.init(&speed);
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
}

CircleMode circle_mode;

void loop() {
  ping.scan();

  mpu9150.loop();
  blinker.execute();
  interpreter.execute();

  unsigned long loop_time_ms = millis();
  loop_count++;

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
        speed_control.set_neutral_pwm_us(rx_speed.pulse_us());
        beeper.beep_ascending();
        Serial.print("switched to automatic");
      }
      if (new_rx_event && rx_events.recent.matches(circle_pattern, count_of(circle_pattern))) {
        mode = mode_circle;
        circle_mode.init(&mpu9150);
        beeper.beep_ascending();
        Serial.print("switched to circle");
      }

      break;

    case mode_automatic:

      steering.writeMicroseconds(1500);
      speed_control.set_command(speed_for_ping_inches(inches));
      speed_control.execute();
      if (new_rx_event && rx_events.current.equals(RxEvent('L','N'))) {
        mode = mode_manual;
        steering.writeMicroseconds(1500);
        speed.writeMicroseconds(1500);
        beeper.beep_descending();
        Serial.println("switched to manual - user initiated");
      }
      if (new_rx_event && rx_events.current.is_bad()) {
        mode = mode_manual;
        steering.writeMicroseconds(1500);
        speed.writeMicroseconds(1500);
        beeper.beep_warble();
        Serial.println("switched to manual - no coms");
      }
      break;

    case mode_circle:
      circle_mode.execute();
      // todo: merge this code into mode manager`
      if (new_rx_event && rx_events.current.equals(RxEvent('L','N'))) {
        mode = mode_manual;
        steering.writeMicroseconds(1500);
        speed.writeMicroseconds(1500);
        beeper.beep_descending();
        Serial.println("switched to manual - user initiated");
      }
      if (new_rx_event && rx_events.current.is_bad()) {
        mode = mode_manual;
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

  // loop speed reporting
  //  12,300 loops per second as of 8/18 9:30
  // 153,400 loops per second on Teensy 3.1 @ 96MHz 8/29
  //  60,900 loops per second on Teensy 3.1 @ 24MHz 8/29
  unsigned long ms_since_report = loop_time_ms - last_report_ms;
  if( ms_since_report >= 1000) {
    unsigned long loops_since_report = loop_count - last_report_loop_count;
    double seconds_since_report =  (loop_time_ms - last_report_ms) / 1000.;
    if(TRACE_LOOP_SPEED) {
          Serial.print("loops per second: ");
          Serial.print( loops_since_report / seconds_since_report );
          Serial.print(" microseconds per loop ");
          Serial.print( 1E6 * seconds_since_report / loops_since_report );
          Serial.println();
    }
    if(TRACE_MPU) {
      mpu9150.trace_status();
    }


    // remember stats for next report
    last_report_ms = loop_time_ms;
    last_report_loop_count = loop_count;
  }

}
