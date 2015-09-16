#include <I2Cdev.h>
#include <Servo.h>
#include "Pins.h"
#include "LogFlags.h"
#include "Blinker.h"
#include "Mpu9150.h"
#include "Car.h"
#include "EventQueue.h"
#include "PwmInput.h"
#include "Esc.h"
#include "Beeper.h"
#include "RxEvents.h"
#include "CircleMode.h"

#define count_of(a) (sizeof(a)/sizeof(a[0]))

#include "Ping.h"

#define PLAY_SOUNDS 0


bool TRACE_RX = false;
bool TRACE_PINGS = false;
bool TRACE_ESC = false;
bool TRACE_MPU = false;
bool TRACE_LOOP_SPEED = false;
bool TRACE_DYNAMICS = true;



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


Esc::eSpeedCommand speed_for_ping_inches(double inches) {
  if(inches ==0) return Esc::speed_neutral;
  // get closer if far
  if (inches > 30.)
    return Esc::speed_forward;
  // back up if too close
  if (inches < 20.)
    return Esc::speed_reverse;

  return Esc::speed_neutral;
}




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
        esc.set_command(Esc::speed_neutral);
        beeper.beep_ascending();
        Serial.print("switched to automatic");
      }
      if (new_rx_event && rx_events.recent.matches(circle_pattern, count_of(circle_pattern))) {
        esc.set_command(Esc::speed_neutral);
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
        esc.set_command(Esc::speed_neutral);
        steering.writeMicroseconds(1500);
        speed.writeMicroseconds(1500);
        beeper.beep_descending();
        Serial.println("switched to manual - user initiated");
      }
      if (new_rx_event && rx_events.current.is_bad()) {
        mode = mode_manual;
        esc.set_command(Esc::speed_neutral);
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
        esc.set_command(Esc::speed_neutral);
        steering.writeMicroseconds(1500);
        speed.writeMicroseconds(1500);
        beeper.beep_descending();
        Serial.println("switched to manual - circle complete");
      }
      if (new_rx_event && rx_events.current.equals(RxEvent('R','N'))) {
        mode = mode_manual;
        esc.set_command(Esc::speed_neutral);
        steering.writeMicroseconds(1500);
        speed.writeMicroseconds(1500);
        beeper.beep_descending();
        Serial.println("switched to manual - user initiated");
      }
      if (new_rx_event && rx_events.current.is_bad()) {
        mode = mode_manual;
        esc.set_command(Esc::speed_neutral);
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
