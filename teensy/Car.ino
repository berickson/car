#include <I2Cdev.h>
#include <Servo.h>
#include "Pins.h"
#include "Logger.h"
#include "Blinker.h"
#include "Mpu9150.h"
#include "Car.h"
#include "EventQueue.h"
#include "PwmInput.h"
#include "Esc.h"
#include "Beeper.h"
#include "RxEvents.h"
#include "CircleMode.h"
#include "ManualMode.h"
#include "FollowMode.h"
#include "RemoteMode.h"
#include "Fsm.h"

#define count_of(a) (sizeof(a)/sizeof(a[0]))

#include "Ping.h"


class CommandInterpreter{
public:
  String buffer;
  const Command * commands;
  int command_count;
  String command_args;

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
      String name = commands[i].name;
      if(s.startsWith(name)) {
        command_args = s.substring(name.length());
        log(LOG_INFO, "Executing command " + name + " with args (" + command_args + ")");
        commands[i].f();
        return;
      }
    }
    log(LOG_ERROR, "Unknown command: ");
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
unsigned long spur_pulse_count = 0;
unsigned long last_spur_pulse_us;
unsigned long microseconds_between_spur_pulse_count;
long odometer=0;
unsigned long last_odometer_change_us=0;



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


const RxEvent auto_pattern [] =
  {{'R','N'},{'C','N'},{'R','N'},{'C','N'},{'R','N'},{'C','N'},{'R','N'},{'C','N'}};
const RxEvent circle_pattern [] =
  {{'L','N'},{'C','N'},{'L','N'},{'C','N'},{'L','N'},{'C','N'},{'L','N'},{'C','N'}};


void motor_rpm_handler() {
  unsigned long us = micros();
  spur_pulse_count++;
  microseconds_between_spur_pulse_count = us - last_spur_pulse_us;
  last_spur_pulse_us = us;
}


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
  TD = true;
}

void trace_dynamics_off() {
  TD = false;
}

extern Fsm modes;

extern CircleMode circle_mode;
void command_circle() {
  double angle;
  String & args = interpreter.command_args;
  log(LOG_TRACE,"circle args" + args);
  if(args.length() == 0) {
    angle = 90.0;
  } else {
    angle = args.toFloat();
  }
  circle_mode.angle_to_turn = angle;
  modes.set_event("circle");
}

void command_remote_control() {
  modes.set_event("remote");
}

extern RemoteMode remote_mode;

void command_pulse_steer_and_esc() {
  String & args = interpreter.command_args;
  log(LOG_TRACE,"pse args" + args);
  int i = args.indexOf(",");
  if(i == -1) {
    log(LOG_ERROR,"invalid args to pse " + args);
    return;
  }
  String s_str = args.substring(0, i);
  String s_esc = args.substring(i+1);

  remote_mode.command_steer_and_esc(s_str.toInt(),  s_esc.toInt());
}

void command_follow() {
  modes.set_event("follow");
}

void command_manual() {
  modes.set_event("manual");
}




void help();



const Command commands[] = {
  {"?", "help", help},
  {"td+", "trace dynamics on", trace_dynamics_on},
  {"td-", "trace dynamics off", trace_dynamics_off},
  {"tp+", "trace ping on", trace_ping_on},
  {"tp-", "trace ping off", trace_ping_off},
  {"te+", "trace esc on", trace_esc_on},
  {"te-", "trace esc off", trace_esc_off},
  {"tm+", "trace mpu on", trace_mpu_on},
  {"tm-", "trace mpu off", trace_mpu_off},
  {"tl+", "trace loop speed on", trace_loop_speed_on},
  {"tl-", "trace loop speed off", trace_loop_speed_off},
  {"c", "circle", command_circle},
  {"m", "manual", command_manual},
  {"f", "follow", command_follow},
  {"rc", "remote control", command_remote_control},
  {"pse", "pulse steer, esc", command_pulse_steer_and_esc}
};

void help() {
  for(unsigned int i = 0; i < count_of(commands); i++) {
    const Command &c = commands[i];
    Serial.println(String(c.name)+ " - " + c.description);
  }
}


Mpu9150 mpu9150;

CircleMode circle_mode;
ManualMode manual_mode;
FollowMode follow_mode;
RemoteMode remote_mode;

Task * tasks[] = {&manual_mode, &circle_mode, &follow_mode, &remote_mode};

Fsm::Edge edges[] = {{"circle", "non-neutral", "manual"},
                     {"circle", "manual", "manual"},
                     {"circle", "done", "manual"},
                     {"follow", "non-neutral", "manual"},
                     {"follow", "manual", "manual"},
                     {"manual", "circle", "circle"},
                     {"manual", "follow", "follow"},
                     {"manual", "remote", "remote"},
                     {"remote", "manual", "manual"},
                     {"remote", "non-neutral", "manual"},
                     {"remote", "done", "manual"}
                  };

Fsm modes(tasks, count_of(tasks), edges, count_of(edges));

void odometer_sensor_a_changed() {
  last_odometer_change_us=micros();
  if(digitalRead(PIN_ODOMOTER_SENSOR_A)==digitalRead(PIN_ODOMOTER_SENSOR_B)){
    --odometer;
  } else {
    ++odometer;
  }
}

void odometer_sensor_b_changed() {
  last_odometer_change_us=micros();
  if(digitalRead(PIN_ODOMOTER_SENSOR_A)==digitalRead(PIN_ODOMOTER_SENSOR_B)){
    ++odometer;
  } else {
    --odometer;
  }
}



void setup() {
  Serial.begin(9600);

  interpreter.init(commands,count_of(commands));
  log(LOG_INFO, "setup begun");

  circle_mode.name = "circle";
  manual_mode.name = "manual";
  follow_mode.name = "follow";
  remote_mode.name = "remote";

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

  pinMode(PIN_MOTOR_RPM, INPUT);
  attachInterrupt(PIN_MOTOR_RPM, motor_rpm_handler, RISING);

  pinMode(PIN_ODOMOTER_SENSOR_A, INPUT);
  pinMode(PIN_ODOMOTER_SENSOR_B, INPUT);
  attachInterrupt(PIN_ODOMOTER_SENSOR_A, odometer_sensor_a_changed, CHANGE);
  attachInterrupt(PIN_ODOMOTER_SENSOR_B, odometer_sensor_b_changed, CHANGE);

  pinMode(PIN_PING_TRIG, OUTPUT);
  pinMode(PIN_PING_ECHO, INPUT);

  digitalWrite(PIN_PING_TRIG, LOW);

  mode = mode_manual;

  beeper.attach(PIN_SPEAKER);

  last_report_ms = millis();

  log(LOG_INFO, "car_control begun");
  mpu9150.setup();
  delay(1000);
  mpu9150.zero();
  circle_mode.init(&mpu9150);
  beeper.beep_nbc();
  modes.begin();
}


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
  bool every_10_ms = every_n_ms(last_loop_time_ms, loop_time_ms, 10);


  // get commands from usb
  interpreter.execute();


  ping.execute();
  mpu9150.execute();
  blinker.execute();
  beeper.execute();

  rx_events.process_pulses(rx_steer.pulse_us(), rx_speed.pulse_us());

  bool new_rx_event = rx_events.get_event();

  double inches = ping.inches();//ping_inches();
  bool new_ping = ping.new_data_ready();
  if(TRACE_PINGS && new_ping) {
    log(TRACE_PINGS, "ping inches: " + inches);
  }

  log(TRACE_RX, "rx_speed: " + rx_speed.pulse_us() + "rx_steer: " + rx_steer.pulse_us());

  // send events through modes state machine
  if(new_rx_event) {
    if(rx_events.recent.matches(circle_pattern, count_of(circle_pattern))) {
      modes.set_event("circle");
    }
    if(rx_events.recent.matches(auto_pattern, count_of(auto_pattern))) {
      modes.set_event("follow");
    }
    if(!rx_events.current.equals(RxEvent('C','N'))) {
      modes.set_event("non-neutral");
    }
  }

  if(every_10_ms) {
    modes.execute();
  }



  // state tracing
  if (0) {
    rx_steer.trace();
    Serial.print(",");
    rx_speed.trace();
    Serial.println();
  }
  if(every_10_ms && TD) {
    
    log(TD,
       "str," + steering.readMicroseconds()
       + ",esc," + speed.readMicroseconds()
       + ",aa,"+ ftos(mpu9150.ax) + "," + ftos(mpu9150.ay)+","+ ftos(mpu9150.az)
       +",spur_us,"+   microseconds_between_spur_pulse_count + "," + last_spur_pulse_us
       +",spur_odo," + spur_pulse_count
       +",ping_mm,"+ping.millimeters()
       +",odo,"+odometer
       +",odo_us," +  last_odometer_change_us
       +",ms,"+millis()
       +",us,"+micros()
       +",ypr,"+ ftos(-mpu9150.yaw* 180. / M_PI) + "," + ftos(-mpu9150.pitch* 180. / M_PI) + "," + ftos(-mpu9150.roll* 180. / M_PI)
       );
  }

  if(every_second && TRACE_MPU ) {
    mpu9150.log_status();
  }

  // loop speed reporting
  //  12,300 loops per second as of 8/18 9:30
  // 153,400 loops per second on Teensy 3.1 @ 96MHz 8/29
  //  60,900 loops per second on Teensy 3.1 @ 24MHz 8/29
  if(every_second && TRACE_LOOP_SPEED) {
    unsigned long loops_since_report = loop_count - last_report_loop_count;
    double seconds_since_report =  (loop_time_ms - last_report_ms) / 1000.;

    log(TRACE_LOOP_SPEED, "loops per second: "+ (loops_since_report / seconds_since_report ) + " microseconds per loop "+ (1E6 * seconds_since_report / loops_since_report) );

    // remember stats for next report
    last_report_ms = loop_time_ms;
    last_report_loop_count = loop_count;
  }

}
