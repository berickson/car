//#include <I2Cdev.h>
#include "Servo2.h"
#include "Pins.h"
#include "QuadratureEncoder.h"
#include "Logger.h"
#include "Blinker.h"
#include "Car.h"
#include "EventQueue.h"
#include "PwmInput.h"
#include "Esc.h"
#include "Beeper.h"
#include "RxEvents.h"
//#include "CircleMode.h"
#include "ManualMode.h"
#include "FollowMode.h"
#include "RemoteMode.h"
#include "Fsm.h"
#include "CarMessages.h"

//#include "BNO055.h"

// all these ugly pushes are becaues the 9150 has a lot of warnings
// the .h file must be included in one time in a source file
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#include <MPU9150_9Axis_MotionApps41.h>
#pragma GCC diagnostic pop


#include "Mpu9150.h"

#define count_of(a) (sizeof(a)/sizeof(a[0]))

#include "Ping.h"
#include "CommandInterpreter.h"






//////////////////////////
// Globals

Servo2 steering;
Servo2 speed;
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


QuadratureEncoder odometer_front_left(PIN_ODOMETER_FRONT_LEFT_SENSOR_A, PIN_ODOMETER_FRONT_LEFT_SENSOR_B);
QuadratureEncoder odometer_front_right(PIN_ODOMETER_FRONT_RIGHT_SENSOR_A, PIN_ODOMETER_FRONT_RIGHT_SENSOR_B);

QuadratureEncoder odometer_back_left(PIN_ODOMETER_BACK_LEFT_SENSOR_A, PIN_ODOMETER_BACK_LEFT_SENSOR_B);
QuadratureEncoder odometer_back_right(PIN_ODOMETER_BACK_RIGHT_SENSOR_A, PIN_ODOMETER_BACK_RIGHT_SENSOR_B);



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

/*
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
*/

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

void command_beep() {
  beeper.beep(beeper.note_c5);

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
//  {"c", "circle", command_circle},
  {"m", "manual", command_manual},
  {"f", "follow", command_follow},
  {"rc", "remote control", command_remote_control},
  {"pse", "pulse steer, esc", command_pulse_steer_and_esc},
  {"beep", "beep", command_beep},
  
};

void help() {
  for(unsigned int i = 0; i < count_of(commands); i++) {
    const Command &c = commands[i];
    Serial.println(String(c.name)+ " - " + c.description);
  }
}


Mpu9150 mpu9150;
//BNO055 mpu_bno;


//CircleMode circle_mode;
ManualMode manual_mode;
FollowMode follow_mode;
RemoteMode remote_mode;

Task * tasks[] = {&manual_mode, /*&circle_mode,*/ &follow_mode, &remote_mode};

Fsm::Edge edges[] = {//{"circle", "non-neutral", "manual"},
                     //{"circle", "manual", "manual"},
                     //{"circle", "done", "manual"},
                     {"follow", "non-neutral", "manual"},
                     {"follow", "manual", "manual"},
  //                   {"manual", "circle", "circle"},
                     {"manual", "follow", "follow"},
                     {"manual", "remote", "remote"},
                     {"remote", "manual", "manual"},
                     {"remote", "non-neutral", "manual"},
                     {"remote", "done", "manual"}
                  };

Fsm modes(tasks, count_of(tasks), edges, count_of(edges));

void odometer_front_left_sensor_a_changed() {
  odometer_front_left.sensor_a_changed();
}

void odometer_front_left_sensor_b_changed() {
  odometer_front_left.sensor_b_changed();
}


void odometer_front_right_sensor_a_changed() {
  odometer_front_right.sensor_a_changed();
}

void odometer_front_right_sensor_b_changed() {
  odometer_front_right.sensor_b_changed();
}


void odometer_back_left_sensor_a_changed() {
  odometer_back_left.sensor_a_changed();
}

void odometer_back_left_sensor_b_changed() {
  odometer_back_left.sensor_b_changed();
}

void odometer_back_right_sensor_a_changed() {
  odometer_back_right.sensor_a_changed();
}

void odometer_back_right_sensor_b_changed() {
  odometer_back_right.sensor_b_changed();
}



void setup() {
  LOG_INFO = false;
  TRACE_MPU = false;
  //trace_dynamics_on();
  LOG_ERROR = true;
  Serial.begin(250000);
  delay(1000);
  //pinMode(13, OUTPUT);
  //int x = 0;
  //while(1) {
  //      ++x;
//	delay(1000);
//	digitalWrite(13, HIGH);
//	delay(1000);
//	digitalWrite(13, LOW);
//	Serial.println("hello");
//	Serial.println(x);
//
//  }
  Wire.begin();
  modes.begin();

  interpreter.init(commands,count_of(commands));
  for(int i = 0; i < 5; i++) {
    delay(1000);
    log(LOG_INFO, "setup begun");
    Serial.println((String)i);
  }


//  circle_mode.name = "circle";
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
  pinMode(PIN_RX_STEER, INPUT);
  pinMode(PIN_RX_SPEED, INPUT);
  pinMode(PIN_GO, INPUT_PULLUP);

  attachInterrupt(int_str, rx_str_handler, CHANGE);
  attachInterrupt(int_esc, rx_spd_handler, CHANGE);

  pinMode(PIN_MOTOR_RPM, INPUT);
  attachInterrupt(PIN_MOTOR_RPM, motor_rpm_handler, RISING);

  pinMode(PIN_ODOMETER_FRONT_LEFT_SENSOR_A, INPUT_PULLUP);
  pinMode(PIN_ODOMETER_FRONT_LEFT_SENSOR_B, INPUT_PULLUP);
  pinMode(PIN_ODOMETER_FRONT_RIGHT_SENSOR_A, INPUT_PULLUP);
  pinMode(PIN_ODOMETER_FRONT_RIGHT_SENSOR_B, INPUT_PULLUP);
  pinMode(PIN_ODOMETER_BACK_LEFT_SENSOR_A, INPUT_PULLUP);
  pinMode(PIN_ODOMETER_BACK_LEFT_SENSOR_B, INPUT_PULLUP);
  pinMode(PIN_ODOMETER_BACK_RIGHT_SENSOR_A, INPUT_PULLUP);
  pinMode(PIN_ODOMETER_BACK_RIGHT_SENSOR_B, INPUT_PULLUP);
  attachInterrupt(PIN_ODOMETER_FRONT_LEFT_SENSOR_A, odometer_front_left_sensor_a_changed, CHANGE);
  attachInterrupt(PIN_ODOMETER_FRONT_LEFT_SENSOR_B, odometer_front_left_sensor_b_changed, CHANGE);
  attachInterrupt(PIN_ODOMETER_FRONT_RIGHT_SENSOR_A, odometer_front_right_sensor_a_changed, CHANGE);
  attachInterrupt(PIN_ODOMETER_FRONT_RIGHT_SENSOR_B, odometer_front_right_sensor_b_changed, CHANGE);
  attachInterrupt(PIN_ODOMETER_BACK_LEFT_SENSOR_A, odometer_back_left_sensor_a_changed, CHANGE);
  attachInterrupt(PIN_ODOMETER_BACK_LEFT_SENSOR_B, odometer_back_left_sensor_b_changed, CHANGE);
  attachInterrupt(PIN_ODOMETER_BACK_RIGHT_SENSOR_A, odometer_back_right_sensor_a_changed, CHANGE);
  attachInterrupt(PIN_ODOMETER_BACK_RIGHT_SENSOR_B, odometer_back_right_sensor_b_changed, CHANGE);

  pinMode(PIN_PING_TRIG, OUTPUT);
  pinMode(PIN_PING_ECHO, INPUT);

  digitalWrite(PIN_PING_TRIG, LOW);

  mode = mode_manual;

  beeper.attach(PIN_SPEAKER);

  last_report_ms = millis();

  log(LOG_INFO, "car_control begun");


  delay(1000);
  /*
    ax_bias: 7724.524414
    ay_bias: -1458.47
    az_bias: 715.62
    zero_adjust(w,x,y,z): (-0.07, 0.67, -0.07, 0.73)
    yaw_slope_degrees_per_hour: -13.823402
  */
  mpu9150.enable_interrupts(PIN_MPU_INTERRUPT);
  log(LOG_INFO, "Interrupts enabled for mpu9150");
  mpu9150.setup();
  log(LOG_INFO, "MPU9150 setup complete");
  mpu9150.ax_bias = 7724.52;
  mpu9150.ay_bias = -1458.47;
  mpu9150.az_bias = 715.62;
  mpu9150.rest_a_mag = 7893.51;
  mpu9150.zero_adjust = Quaternion(-0.07, 0.67, -0.07, 0.73);
  // was ((-13.823402+4.9) / (1000 * 60 * 60)) * PI/180;
  mpu9150.yaw_slope_rads_per_ms  = (2.7 / (10 * 60 * 1000)) * PI / 180;
  mpu9150.yaw_actual_per_raw = (3600. / (3600 - 29.0 )); //1.0; // (360.*10.)/(360.*10.-328);// 1.00; // 1.004826221;

  mpu9150.zero_heading();


  //mpu_bno.setTempSource(true);
  //mpu_bno.begin(POWER_MODE::NORMAL, OPERATION_MODE::FUSION_NDOF_NORMAL, PLACEMENT::P4);
  //mpu_bno.loadCalib();

  delay(1000);
  //mpu9150.zero();
  //circle_mode.init(&mpu9150);
  beeper.beep_nbc();
  modes.begin();
}


// returns true if loop time passes through n ms boundary
bool every_n_ms(unsigned long last_loop_ms, unsigned long loop_ms, unsigned long ms) {
  return (last_loop_ms % ms) + (loop_ms - last_loop_ms) >= ms;
}


struct Eulers {
  float yaw;
  float pitch;
  float roll;
};

struct Acceleration {
  float ax;
  float ay;
  float az;
};


void loop() {
  // set global loop values
  loop_count++;
  last_loop_time_ms = loop_time_ms;
  loop_time_ms = millis();

  // get common execution times
  bool every_second = every_n_ms(last_loop_time_ms, loop_time_ms, 1000);
  bool every_10_ms = every_n_ms(last_loop_time_ms, loop_time_ms, 10);
  //bool every_1_ms = every_n_ms(last_loop_time_ms, loop_time_ms, 1);


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

  if(every_10_ms && TD) {
    // constants below based on 220k and 1M resistor, 1023 steps and 3.3 reference voltage
    float battery_voltage = analogRead(PIN_BATTERY_VOLTAGE_DIVIDER) * ((3.3/1023.) / 220.)*(220.+1000.);
    
    noInterrupts();
    static TraceDynamics v;
    v.odo_fl_a = odometer_front_left.odometer_a;
    v.odo_fl_a_us = odometer_front_left.last_odometer_a_us;
    v.odo_fl_b = odometer_front_left.odometer_b;
    v.odo_fl_b_us = odometer_front_left.last_odometer_b_us;
    v.odo_fl_ab_us = odometer_front_left.odometer_ab_us;

    v.odo_fr_a = odometer_front_right.odometer_a;
    v.odo_fr_a_us = odometer_front_right.last_odometer_a_us;
    v.odo_fr_b = odometer_front_right.odometer_b;
    v.odo_fr_b_us = odometer_front_right.last_odometer_b_us;
    v.odo_fr_ab_us = odometer_front_right.odometer_ab_us;
    
    v.odo_bl_a = odometer_back_left.odometer_a;
    v.odo_bl_a_us = odometer_back_left.last_odometer_a_us;
    v.odo_bl_b = odometer_back_left.odometer_b;
    v.odo_bl_b_us = odometer_back_left.last_odometer_b_us;
    v.odo_bl_ab_us = odometer_back_left.odometer_ab_us;
    
    v.odo_br_a = odometer_back_right.odometer_a;
    v.odo_br_a_us = odometer_back_right.last_odometer_a_us;
    v.odo_br_b = odometer_back_right.odometer_b;
    v.odo_br_b_us = odometer_back_right.last_odometer_b_us;
    v.odo_br_ab_us = odometer_back_right.odometer_ab_us;
    v.go = digitalRead(PIN_GO) == HIGH ? 1 : 0;

    auto fl_odo_a = odometer_front_left.odometer_a;
    auto fl_odo_a_us = odometer_front_left.last_odometer_a_us;
    auto fl_odo_b = odometer_front_left.odometer_b;
    auto fl_odo_b_us = odometer_front_left.last_odometer_b_us;
    auto fl_odo_ab_us = odometer_front_left.odometer_ab_us;

    auto fr_odo_a = odometer_front_right.odometer_a;
    auto fr_odo_a_us = odometer_front_right.last_odometer_a_us;
    auto fr_odo_b = odometer_front_right.odometer_b;
    auto fr_odo_b_us = odometer_front_right.last_odometer_b_us;
    auto fr_odo_ab_us = odometer_front_right.odometer_ab_us;
    
    auto bl_odo_a = odometer_back_left.odometer_a;
    auto bl_odo_a_us = odometer_back_left.last_odometer_a_us;
    auto bl_odo_b = odometer_back_left.odometer_b;
    auto bl_odo_b_us = odometer_back_left.last_odometer_b_us;
    auto bl_odo_ab_us = odometer_back_left.odometer_ab_us;
    
    auto br_odo_a = odometer_back_right.odometer_a;
    auto br_odo_a_us = odometer_back_right.last_odometer_a_us;
    auto br_odo_b = odometer_back_right.odometer_b;
    auto br_odo_b_us = odometer_back_right.last_odometer_b_us;
    auto br_odo_ab_us = odometer_back_right.odometer_ab_us;
    int go = digitalRead(PIN_GO) == HIGH ? 1 : 0;

    interrupts();
    StringOutTransfer doc;
    v.transfer(doc);
    bool T2=true;
    log(T2, doc.str());

    
    log(TD,
       "str," + steering.readMicroseconds()
       + ",esc," + speed.readMicroseconds()
       + ",aa,"+ ftos(mpu9150.ax) + "," + ftos(mpu9150.ay)+","+ ftos(mpu9150.az)
       +",spur_us,"+   microseconds_between_spur_pulse_count + "," + last_spur_pulse_us
       +",spur_odo," + mpu9150.temperature // spur_pulse_count
       +",mode,"+modes.current_task->name
       +",odo_fl,"+ fl_odo_a +"," +  fl_odo_a_us + "," + fl_odo_b +"," + fl_odo_b_us + "," + fl_odo_ab_us
       +",odo_fr,"+ fr_odo_a +"," +  fr_odo_a_us + "," + fr_odo_b +"," + fr_odo_b_us + "," + fr_odo_ab_us
       +",odo_bl,"+ bl_odo_a +"," +  bl_odo_a_us + "," + bl_odo_b +"," + bl_odo_b_us + "," + bl_odo_ab_us
       +",odo_br,"+ br_odo_a +"," +  br_odo_a_us + "," + br_odo_b +"," + br_odo_b_us + "," + br_odo_ab_us
       +",ms,"+millis()
       +",us,"+micros()
       +",ypr,"+ mpu9150.heading() + "," + ftos(mpu9150.pitch* 180. / M_PI) + "," + ftos(-mpu9150.roll* 180. / M_PI)
       +",vbat,"+ftos(battery_voltage)
       +",go,"+go
       );
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
