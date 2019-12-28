#include "Arduino.h"
#include "Task.h"
#include "Fsm.h"
#include "ManualMode.h"
#include "RemoteMode.h"
#include "CommandInterpreter.h"
#include "RxEvents.h"
#include "Blinker.h"
#include "CarMessages.h"


#if defined(BLUE_CAR)
const int pin_led = 13;
const int pin_motor_a = 14;
const int pin_motor_b = 12;
const int pin_motor_c = 11;
//const int pin_motor_temp = A13;

const int pin_odo_fl_a = 23;
const int pin_odo_fl_b = 22;
const int pin_odo_fr_a = 21;
const int pin_odo_fr_b = 20;

const int pin_str = 26;
const int pin_esc = 27;
const int pin_esc_aux = 15;

const int pin_rx_str = 25;
const int pin_rx_esc = 24;

const int pin_mpu_interrupt = 17;

const int pin_vbat_sense = A13;
const int pin_cell1_sense = A14;
const int pin_cell2_sense = A15;
const int pin_cell3_sense = A16;
const int pin_cell4_sense = A17;
const int pin_cell0_sense = A18;

#elif defined(ORANGE_CAR)
#pragma message("ORANGE_CAR defined")

// orange-crash board layout at
// https://docs.google.com/drawings/d/162KwWO05w_nAkaWieq2LWwWhbdojVjPIpCre_7vMy38/edit



#define PIN_GO 16

// "A" means toward front
#define PIN_MOTOR_RPM 29

#define PIN_SPEAKER   30


const int pin_led = 13;
const int pin_vbat_sense = 14;
const int pin_cell1_sense = 14;
const int pin_cell2_sense = 14;
const int pin_cell3_sense = 14;
const int pin_cell4_sense = 14;
const int pin_cell0_sense = 14;

const int pin_motor_a = 17;
const int pin_motor_b = 11;
const int pin_motor_c = 32;
const int pin_motor_temp = A13;


#define pin_odo_bl_a 10
#define pin_odo_bl_b 10
#define pin_odo_br_a 10
#define pin_odo_br_b 10


#define pin_odo_fl_a 8
#define pin_odo_fl_b 25
#define pin_odo_fr_a 26
#define pin_odo_fr_b 9

const int pin_str = 5;
const int pin_esc = 6;
const int pin_esc_aux = 10;

const int pin_rx_str = 3;
const int pin_rx_esc = 4;

const int pin_mpu_interrupt = 33;
#else
#error "You must define BLUE_CAR or ORANGE_CAR"
#endif

// all these ugly pushes are because the 9150 has a lot of warnings
// the .h file must be included in one time in a source file
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#include <MPU9150_9Axis_MotionApps41.h>
#pragma GCC diagnostic pop

#include "Mpu9150.h"

Mpu9150 mpu9150;

#include "QuadratureEncoder.h"
#include "MotorEncoder.h"

#include "Servo2.h"
#include "PwmInput.h"

///////////////////////////////////////////////
// helpers

#define count_of(a) (sizeof(a)/sizeof(a[0]))


///////////////////////////////////////////////
// globals

QuadratureEncoder odo_fl(pin_odo_fl_a, pin_odo_fl_b);
QuadratureEncoder odo_fr(pin_odo_fr_a, pin_odo_fr_b);

#if defined(pin_odo_bl_a)
QuadratureEncoder odo_bl(pin_odo_bl_a, pin_odo_bl_b);
#endif

#if defined(pin_odo_br_a)
QuadratureEncoder odo_br(pin_odo_br_a, pin_odo_br_b);
#endif


PwmInput rx_str;
PwmInput rx_esc;
RxEvents rx_events;


Servo2 str;
Servo2 esc;

MotorEncoder motor(pin_motor_a, pin_motor_b, pin_motor_c);

Blinker blinker;

///////////////////////////////////////////////
// modes

ManualMode manual_mode;
RemoteMode remote_mode;

Task * tasks[] = {&manual_mode, &remote_mode};

Fsm::Edge edges[] = {
  {"manual", "remote", "remote"},
  {"remote", "manual", "manual"},
  {"remote", "non-neutral", "manual"},
  {"remote", "done", "manual"}
};

Fsm modes(tasks, count_of(tasks), edges, count_of(edges));

///////////////////////////////////////////////
// commands

CommandInterpreter interpreter;

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

void trace_telemetry_on() {
  TRACE_TELEMETRY = true;
}

void trace_telemetry_off() {
  TRACE_TELEMETRY = false;
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




void command_manual() {
  modes.set_event("manual");
}

void command_remote_control() {
  modes.set_event("remote");
  Serial.println("remote event");
}

void calibrate_mpu() {
  mpu9150.start_calibrate_at_rest(0, 60);
}

void help(); // forward decl for commands

const Command commands[] = {
  {"?", "help", help},
  {"help", "help", help},
  {"calibrate", "calibrate at rest mpu", calibrate_mpu},
  {"td+", "trace dynamics on", trace_dynamics_on},
  {"td-", "trace dynamics off", trace_dynamics_off},
//  {"tp+", "trace ping on", trace_ping_on},
//  {"tp-", "trace ping off", trace_ping_off},
  {"tt+", "trace telemetry on", trace_telemetry_on},
  {"tt-", "trace telemetry on", trace_mpu_off},
  {"tm+", "trace mpu on", trace_mpu_on},
  {"tm-", "trace mpu off", trace_mpu_off},
  {"tl+", "trace loop speed on", trace_loop_speed_on},
  {"tl-", "trace loop speed off", trace_loop_speed_off},
//  {"c", "circle", command_circle},
  {"m", "manual", command_manual},
//  {"f", "follow", command_follow},
  {"rc", "remote control", command_remote_control},
  {"pse", "pulse steer, esc", command_pulse_steer_and_esc},
//  {"beep", "beep", command_beep},
  
};

void help() {
  for(unsigned int i = 0; i < count_of(commands); i++) {
    const Command &c = commands[i];
    Serial.println(String(c.name)+ " - " + c.description);
  }
}


class BatterySensor {
public:

  int resolution_bits = 10;
  float r1 = 102;
  float r2 = 422;
  float max_reading = 1024;
  float scale = (r1+r2) * 3.3 / (max_reading * r2);

  float v_bat;
  float v_cell0;
  float v_cell1;
  float v_cell2;
  float v_cell3;
  float v_cell4;


  void init() {
    analogReadResolution(resolution_bits);	
    max_reading = pow(2, resolution_bits);
    scale = (r1+r2) * 3.3 / (max_reading * r1);
  }

  void execute() {
    v_bat = analogRead(pin_vbat_sense) * scale * 12.41 / 12.25;
    v_cell0 = analogRead(pin_cell0_sense) * scale * 3.343/3.28;
    v_cell1 = analogRead(pin_cell1_sense)  * scale * 3.343/3.29;
    v_cell2 = analogRead(pin_cell2_sense)  * scale * 3.343/3.33;
    v_cell3 = analogRead(pin_cell3_sense) * scale * 3.343/3.29;
    v_cell4 = analogRead(pin_cell4_sense) * scale * 3.343/3.29;
    String s = (String) "vbat: " + v_bat 
                    + " v_cell0: " +  v_cell0 
                    + " v_cell1: " +  v_cell1 
                    + " v_cell2: "+  v_cell2 
                    + " v_cell3: "+  v_cell3 
                    + " v_cell4: "+  v_cell4;
    log(LOG_INFO, s);
  }


};

BatterySensor battery_sensor;


///////////////////////////////////////////////
// Interrupt handlers

void rx_str_handler() {
  rx_str.handle_change();
}

void rx_esc_handler() {
  rx_esc.handle_change();
}


void odo_bl_a_changed() {
  odo_bl.sensor_a_changed();
}

void odo_bl_b_changed() {
  odo_bl.sensor_b_changed();
}

void odo_br_a_changed() {
  odo_br.sensor_a_changed();
}

void odo_br_b_changed() {
  odo_br.sensor_b_changed();
}

void odo_fl_a_changed() {
  odo_fl.sensor_a_changed();
}

void odo_fl_b_changed() {
  odo_fl.sensor_b_changed();
}

void odo_fr_a_changed() {
  odo_fr.sensor_a_changed();
}

void odo_fr_b_changed() {
  odo_fr.sensor_b_changed();
}

void motor_a_changed() {
  motor.on_a_change();
}

void motor_b_changed() {
  motor.on_b_change();
}

void motor_c_changed() {
  motor.on_c_change();
}


bool every_n_ms(unsigned long last_loop_ms, unsigned long loop_ms, unsigned long ms) {
  return (last_loop_ms % ms) + (loop_ms - last_loop_ms) >= ms;
}


void setup() {

  Serial.begin(250000);
  //while(!Serial); // wait for serial to become ready
  delay(1000);
 

  // put your setup code here, to run once:
  rx_str.attach(pin_rx_str);
  rx_esc.attach(pin_rx_esc);
  str.attach(pin_str);
  esc.attach(pin_esc);

  // 1500 is usually safe...
  esc.writeMicroseconds(1500);
  str.writeMicroseconds(1500);
  manual_mode.name = "manual";
  // follow_mode.name = "follow";
  remote_mode.name = "remote";

  interpreter.init(commands, count_of(commands));
  modes.begin();

  pinMode(pin_motor_a, INPUT);
  pinMode(pin_motor_b, INPUT);
  pinMode(pin_motor_c, INPUT);


  attachInterrupt(pin_motor_a, motor_a_changed, CHANGE);
  attachInterrupt(pin_motor_b, motor_b_changed, CHANGE);
  attachInterrupt(pin_motor_c, motor_c_changed, CHANGE);

  // pinmode is required for Teensy3.2?
  pinMode(pin_odo_fl_a, INPUT);
  pinMode(pin_odo_fl_b, INPUT);
  pinMode(pin_odo_fr_a, INPUT);
  pinMode(pin_odo_fr_a, INPUT);
  attachInterrupt(pin_odo_fl_a, odo_fl_a_changed, CHANGE);
  attachInterrupt(pin_odo_fl_b, odo_fl_b_changed, CHANGE);
  attachInterrupt(pin_odo_fr_a, odo_fr_a_changed, CHANGE);
  attachInterrupt(pin_odo_fr_b, odo_fr_b_changed, CHANGE);

  #if defined(pin_odo_bl_a)
  pinMode(pin_odo_bl_a, INPUT);
  pinMode(pin_odo_bl_b, INPUT);
  attachInterrupt(pin_odo_bl_a, odo_bl_a_changed, CHANGE);
  attachInterrupt(pin_odo_bl_b, odo_bl_b_changed, CHANGE);
  #endif

  #if defined(pin_odo_br_a)
  pinMode(pin_odo_br_a, INPUT);
  pinMode(pin_odo_br_b, INPUT);
  attachInterrupt(pin_odo_br_a, odo_br_a_changed, CHANGE);
  attachInterrupt(pin_odo_br_b, odo_br_b_changed, CHANGE);
  #endif


  // pinMode(pin_motor_temp, INPUT_PULLUP);

  attachInterrupt(pin_rx_str, rx_str_handler, CHANGE);
  attachInterrupt(pin_rx_esc, rx_esc_handler, CHANGE);

  log(LOG_TRACE,"starting wire");
  Wire.begin();

  log(LOG_TRACE, "starting mpu");

  mpu9150.enable_interrupts(pin_mpu_interrupt);
  log(LOG_INFO, "Interrupts enabled for mpu9150");
  mpu9150.setup();
  
  mpu9150.ax_bias = 0; // 7724.52;
  mpu9150.ay_bias = 0; // -1458.47;
  mpu9150.az_bias = 7893.51; // 715.62;
  mpu9150.rest_a_mag =  7893.51;
  mpu9150.zero_adjust = Quaternion(0.0, 0.0, 0.0, 1);// Quaternion(-0.07, 0.67, -0.07, 0.73);
  // was ((-13.823402+4.9) / (1000 * 60 * 60)) * PI/180;
  mpu9150.yaw_slope_rads_per_ms  = -0.0000000680;// (2.7 / (10 * 60 * 1000)) * PI / 180;
  mpu9150.yaw_actual_per_raw = 1; //(3600. / (3600 - 29.0 )); //1.0; // (360.*10.)/(360.*10.-328);// 1.00; // 1.004826221;

  mpu9150.zero_heading();
  blinker.init(pin_led);
  battery_sensor.init();
  log(LOG_INFO, "setup complete");
}

// diagnostics for reporting loop speeds
unsigned long loop_count = 0;
unsigned long loop_time_ms = 0;
unsigned long last_loop_time_ms = 0;
unsigned long last_report_ms = 0;
unsigned long last_report_loop_count = 0;

void loop() {
  loop_count++;
  last_loop_time_ms = loop_time_ms;
  loop_time_ms = millis();

  bool every_second = every_n_ms(last_loop_time_ms, loop_time_ms, 1000);
  bool every_10_ms = every_n_ms(last_loop_time_ms, loop_time_ms, 10);


  blinker.execute();
  mpu9150.execute();
  interpreter.execute();

  rx_events.process_pulses(rx_str.pulse_us(), rx_esc.pulse_us());
  bool new_rx_event = rx_events.get_event();
  // send events through modes state machine
  if(new_rx_event) {
    if(!rx_events.current.equals(RxEvent('C','N'))) {
      modes.set_event("non-neutral");
    }
  }  


  if (every_10_ms) {
    modes.execute();
  }

  if(every_second) {
    battery_sensor.execute();
  }

  if (every_second && TRACE_TELEMETRY) {

      log(TRACE_TELEMETRY,
        (String) "odo:" + motor.odometer 
        + " odo_us: " + motor.last_change_us
        + " a: " + motor.a_count 
        + " b: " + motor.b_count 
        + " c: " + motor.c_count 
//        + " temp: " + analogRead(pin_motor_temp)
        + " heading: " + mpu9150.heading()
        + " fl: (" + odo_fl.odometer_a + ","+odo_fl.odometer_b+")"
        + " fr: (" + odo_fr.odometer_a + ","+odo_fr.odometer_b+")"
      );
  }

  if(every_10_ms && TD) {
    Dynamics2 td2;
    float battery_voltage = battery_sensor.v_bat;

    td2.ms = millis();
    td2.us = micros();

    td2.rx_str = str.readMicroseconds();
    td2.rx_esc = esc.readMicroseconds();

    td2.ax = mpu9150.ax;
    td2.ay = mpu9150.ay;
    td2.az = mpu9150.az;

    noInterrupts();
    td2.spur_us = motor.last_change_us;
    td2.spur_odo = motor.odometer;
    interrupts();

    td2.mode = modes.current_task->name;

    noInterrupts();
    td2.odo_fl_a = odo_fl.odometer_a;
    td2.odo_fl_a_us = odo_fl.last_odometer_a_us;
    td2.odo_fl_b = odo_fl.odometer_b;
    td2.odo_fl_b_us = odo_fl.last_odometer_b_us;
    td2.odo_fl_ab_us = odo_fl.odometer_ab_us;
    interrupts();

    noInterrupts();
    td2.odo_fr_a = odo_fr.odometer_a;
    td2.odo_fr_a_us = odo_fr.last_odometer_a_us;
    td2.odo_fr_b = odo_fr.odometer_b;
    td2.odo_fr_b_us = odo_fr.last_odometer_b_us;
    td2.odo_fr_ab_us = odo_fr.odometer_ab_us;
    interrupts();

    noInterrupts();
    td2.odo_bl_a = odo_fl.odometer_a;
    td2.odo_bl_a_us = odo_fl.last_odometer_a_us;
    td2.odo_bl_b = odo_fl.odometer_b;
    td2.odo_bl_b_us = odo_fl.last_odometer_b_us;
    td2.odo_bl_ab_us = odo_fl.odometer_ab_us;
    interrupts();

    noInterrupts();
    td2.odo_br_a = odo_fr.odometer_a;
    td2.odo_br_a_us = odo_fr.last_odometer_a_us;
    td2.odo_br_b = odo_fr.odometer_b;
    td2.odo_br_b_us = odo_fr.last_odometer_b_us;
    td2.odo_br_ab_us = odo_fr.odometer_ab_us;
    interrupts();

    td2.v_bat = battery_voltage;

    td2.mpu_deg_yaw = mpu9150.heading();
    td2.mpu_deg_pitch = mpu9150.pitch * 180. / M_PI;
    td2.mpu_deg_roll = mpu9150.roll * 180. / M_PI;

    td2.mpu_deg_f = mpu9150.temperature /340.0 + 35.0;

    // hack: car doesn't have kill switch yet
    td2.go = true;


    StringOutTransfer stream;
    td2.transfer(stream);

    log(TD2, stream.str());

  }

  if(every_second && TRACE_LOOP_SPEED) {
    unsigned long loops_since_report = loop_count - last_report_loop_count;
    float seconds_since_report =  (loop_time_ms - last_report_ms) / 1000.;

    log(TRACE_LOOP_SPEED, 
      "loops per second: "+ (loops_since_report / seconds_since_report ) 
      + " microseconds per loop "+ (1E6 * seconds_since_report / loops_since_report) );

    // remember stats for next report
    last_report_ms = loop_time_ms;
    last_report_loop_count = loop_count;
  }
}
