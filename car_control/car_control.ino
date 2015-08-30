#include <Wire.h>

#include <Servo.h>
#include "mpu9150.h"

#define count_of(a) (sizeof(a)/sizeof(a[0]))

/////////////////////////////////////////////////////
// PIN / Wiring
//
// on the micro, interrupts must go on pins 2 and 3
// interrupt 0 = pin 2
// interrupt 1 = pin 3
#define PIN_RX_STEER 2
#define PIN_RX_SPEED 3
#define PIN_SRC_SELECT 4

#define PIN_PING_TRIG 6
#define PIN_PING_ECHO 7

#define PIN_SPEAKER   10

// computer steering and speed
#define PIN_U_STEER 8
#define PIN_U_SPEED 9



struct PwmInput {
  unsigned long last_trigger_us = 0;
  unsigned long pulse_width_us = 0;

  // since we are dealing with standard RC,
  // anything out of of the below ranges should not occur and is ignored
  unsigned long max_pulse_us = 3000;
  unsigned long min_pulse_us = 500;

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

  void beep(int note) {
    tone(pin, note, note_ms);
    delay(note_ms + gap_ms);
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
  static const int size = 5;

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

  unsigned long ping_rate_ms = 50;
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

  double inches() {
    return last_ping_distance_inches;
  }
};


struct SpeedControl {

  Servo * speed;

  const unsigned long brake_ms = 500;
  const unsigned long pause_ms = 200;
  unsigned long brake_start_ms = 0;
  unsigned long pause_start_ms = 0;

  const int forward_us =  1350;
  const int reverse_us =  1650;
  const int neutral_us = 1500;
  int calibration_us = 0;

  char command = 'N';

  enum {
    stopped,
    forward_braking,
    reverse_braking,
    forward,
    reverse,
    pausing
  } state;

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
    unsigned long c_us = us + calibration_us;
    speed->writeMicroseconds(c_us);
  }

  void set_command(char speed_code) {
    Serial.println(speed_code);
    if(speed_code == 'F') {
      if(state == stopped || state == forward_braking) {
        state = forward;
        set_pwm_us(forward_us);
      }
      if(state == reverse) {
        set_pwm_us(forward_us);
        state = reverse_braking;
        brake_start_ms = millis();
      }
    }
    if(speed_code == 'R') {
      if(state == stopped || state == reverse_braking) {
        state = reverse;
        set_pwm_us(reverse_us);
      }
      if(state == forward) {
        state = forward_braking;
        set_pwm_us(reverse_us);
        brake_start_ms = millis();
      }
    }
    if(speed_code == 'N'){
      if(state == forward){
        state = forward_braking;
        set_pwm_us(reverse_us);
        brake_start_ms = millis();
      }
      if(state == reverse){
        state = reverse_braking;
        set_pwm_us(forward_us);
        brake_start_ms = millis();
      }
    }

    command = speed_code;
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
        if(command == 'F') {
          set_pwm_us(forward_us);
          state = forward;
        }
        if(command == 'R') {
          set_pwm_us(reverse_us);
          state = reverse;
        }
        if(command == 'N') {
          set_pwm_us(neutral_us);
          state = stopped;
        }
      }
    }
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


// diagnostics for reporting loop speeds
unsigned long loop_count = 0;
unsigned long last_report_ms = 0;
unsigned long last_report_loop_count = 0;


enum {
  mode_manual,
  mode_automatic
} mode;


///////////////////////////////////////////////
// Interrupt handlers

void rx_str_handler() {
  rx_steer.handle_change();
}

void rx_spd_handler() {
  rx_speed.handle_change();
}

char speed_for_ping_inches(double inches) {
  // get closer if far
  if (inches > 25.)
    return 'F';
  // back up if too close
  if (inches < 12.)
    return 'R';

  return 'N';
}


const RxEvent auto_pattern [] =
  {{'R','N'},{'C','N'},{'R','N'},{'C','N'},{'R','N'}};


Mpu9150 mpu9150;

void setup() {
  steering.attach(PIN_U_STEER);
  speed.attach(PIN_U_SPEED);


  rx_steer.attach(PIN_RX_STEER);
  rx_speed.attach(PIN_RX_SPEED);
  speed_control.init(&speed);

  pinMode(PIN_SRC_SELECT, OUTPUT);
  digitalWrite(PIN_SRC_SELECT, LOW);

  ping.init(PIN_PING_TRIG, PIN_PING_ECHO);

  attachInterrupt(0, rx_str_handler, CHANGE);
  attachInterrupt(1, rx_spd_handler, CHANGE);


  pinMode(PIN_PING_TRIG, OUTPUT);
  pinMode(PIN_PING_ECHO, INPUT);

  digitalWrite(PIN_PING_TRIG, LOW);

  mode = mode_manual;

  beeper.attach(PIN_SPEAKER);
  beeper.beep_nbc();

  last_report_ms = millis();

  Serial.begin(9600);
  Serial.println("car_control begun");
  mpu9150.setup();
}


void loop() {
  mpu9150.loop();

  unsigned long loop_time_ms = millis();
  loop_count++;

  rx_events.process_pulses(rx_steer.pulse_us(), rx_speed.pulse_us());
  bool new_rx_event = rx_events.get_event();
  ping.scan();

  double inches = ping.inches();//ping_inches();
  if(0 && ping.new_data_ready()) {
    Serial.print("ping inches:");
    Serial.println(inches);
  }



  switch (mode) {
    case mode_manual:
      steering.writeMicroseconds(1500);
      speed.writeMicroseconds(1500);
      digitalWrite(PIN_SRC_SELECT, LOW);
      if (new_rx_event && rx_events.recent.matches(auto_pattern, count_of(auto_pattern))) {
        mode = mode_automatic;
        speed_control.set_neutral_pwm_us(rx_speed.pulse_us());
        beeper.beep_ascending();
        Serial.print("switched to automatic");
      }
      break;

    case mode_automatic:
      digitalWrite(PIN_SRC_SELECT, HIGH);
      steering.writeMicroseconds(1500);
      speed_control.set_command(speed_for_ping_inches(inches));
      speed_control.execute();
      if (new_rx_event && rx_events.current.equals(RxEvent('L','N'))) {
        mode = mode_manual;
        steering.writeMicroseconds(1500);
        speed.writeMicroseconds(1500);
        digitalWrite(PIN_SRC_SELECT, LOW);
        beeper.beep_descending();
        Serial.println("switched to manual - user initiated");
      }
      if (new_rx_event && rx_events.current.is_bad()) {
        mode = mode_manual;
        steering.writeMicroseconds(1500);
        speed.writeMicroseconds(1500);
        digitalWrite(PIN_SRC_SELECT, LOW);
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
  if(1) {
    unsigned long ms_since_report = loop_time_ms - last_report_ms;
    if( ms_since_report >= 10000) {
      unsigned long loops_since_report = loop_count - last_report_loop_count;
      double seconds_since_report =  (loop_time_ms - last_report_ms) / 1000.;
      Serial.print("loops per second: ");
      Serial.print( loops_since_report / seconds_since_report );
      Serial.print(" microseconds per loop");
      Serial.print( 1E6 * seconds_since_report / loops_since_report );
      Serial.println();

      // remember stats for next report
      last_report_ms = loop_time_ms;
      last_report_loop_count = loop_count;
    }
  }

}
