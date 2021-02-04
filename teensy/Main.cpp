#include "Arduino.h"
#include "Task.h"
#include "Fsm.h"
#include "ManualMode.h"
#include "RemoteMode.h"
#include "RxEvents.h"
#include "Blinker.h"

// https://platformio.org/lib/show/1634/Rosserial%20Arduino%20Library
#include <ros.h>
#include <std_msgs/Bool.h>
#include <car_msgs/update.h>
#include <car_msgs/rc_command.h>

ros::NodeHandle  nh;

car_msgs::update update_msg;
ros::Publisher  car_update("car/update", &update_msg);


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

// "A" means toward front2
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


void command_manual() {
  modes.set_event("manual");
}

void command_remote_control() {
  modes.set_event("remote");
}

void calibrate_mpu() {
  mpu9150.start_calibrate_at_rest(0, 60);
}

class BatterySensor {
public:

  int resolution_bits = 10;
  float r1 = 102;
  float r2 = 422;
  float max_reading = 1024;
  float scale = (r1+r2) * 3.3 / (max_reading * r2);

  float v_bat = 0;
  float v_cell0 = 0;
  float v_cell1 = 0;
  float v_cell2 = 0;
  float v_cell3 = 0;
  float v_cell4 = 0;


  void init() {
    analogReadResolution(resolution_bits);	
    max_reading = pow(2, resolution_bits);
    scale = (r1+r2) * 3.3 / (max_reading * r1);
  }

  void execute() {
#if defined(BLUE_CAR)
    v_bat = analogRead(pin_vbat_sense) * scale * 12.41 / 12.25;
    v_cell0 = analogRead(pin_cell0_sense) * scale * 3.343/3.28;
    v_cell1 = analogRead(pin_cell1_sense)  * scale * 3.343/3.29;
    v_cell2 = analogRead(pin_cell2_sense)  * scale * 3.343/3.33;
    v_cell3 = analogRead(pin_cell3_sense) * scale * 3.343/3.29;
    v_cell4 = analogRead(pin_cell4_sense) * scale * 3.343/3.29;
#elif defined(ORANGE_CAR)
    // constants below based on 220k and 1M resistor, 1023 steps and 3.3 reference voltage
    v_bat = analogRead(pin_vbat_sense) * ((3.3/1023.) / 220.)*(220.+1000.);
#else
#error "voltage not defined for this car"
#endif
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


void rc_command_callback( const car_msgs::rc_command& rc_command_msg){
  remote_mode.command_steer_and_esc(rc_command_msg.str_us,  rc_command_msg.str_us);
}

void enable_remote_control_callback( const std_msgs::Bool& enable_rc_msg) {
  if(enable_rc_msg.data == true) {
      modes.set_event("remote");
  } else {
      modes.set_event("manual");
  }
}

ros::Subscriber<car_msgs::rc_command> rc_command_sub("car/rc_command", &rc_command_callback );
ros::Subscriber<std_msgs::Bool> enable_rc_mode("car/enable_rc_mode", &enable_remote_control_callback);


void setup() {

  Serial.begin(250000);
  //while(!Serial); // wait for serial to become ready
  delay(1000);

  nh.initNode();
  nh.advertise(car_update);
  nh.subscribe(rc_command_sub);
  nh.subscribe(enable_rc_mode);
 

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

  // interpreter.init(commands, count_of(commands));
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



  // pinMode(pin_motor_temp, INPUT_PULLUP);

  attachInterrupt(pin_rx_str, rx_str_handler, CHANGE);
  attachInterrupt(pin_rx_esc, rx_esc_handler, CHANGE);

  log(LOG_TRACE,"starting wire");
  Wire.begin();

  log(LOG_TRACE, "starting mpu");

  mpu9150.enable_interrupts(pin_mpu_interrupt);
  log(LOG_INFO, "Interrupts enabled for mpu9150");
  mpu9150.setup();
#if defined(BLUE_CAR)
  mpu9150.ax_bias = 0;
  mpu9150.ay_bias = 0;
  mpu9150.az_bias = 7893.51;
  mpu9150.rest_a_mag =  7893.51;
  mpu9150.zero_adjust = Quaternion(0.707, 0.024, -0.024, 0.707);
  mpu9150.yaw_slope_rads_per_ms  = -0.0000000680;
  mpu9150.yaw_actual_per_raw = 1;
#elif defined(ORANGE_CAR)
  mpu9150.ax_bias = 7724.52;
  mpu9150.ay_bias = -1458.47;
  mpu9150.az_bias = 715.62;
  mpu9150.rest_a_mag = 7893.51;
  mpu9150.zero_adjust = Quaternion(-0.07, 0.67, -0.07, 0.73);
  mpu9150.yaw_slope_rads_per_ms  = (2.7 / (10 * 60 * 1000)) * PI / 180;
  mpu9150.yaw_actual_per_raw = (3600. / (3600 - 29.0 )); //1.0; // (360.*10.)/(360.*10.-328);// 1.00; // 1.004826221;
#else
#error "Car not defined for MPU"
#endif
  mpu9150.zero_heading();
  blinker.init(pin_led);
  battery_sensor.init();
  log(LOG_INFO, "setup complete");
  nh.loginfo("teensy setup complete");
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
  // interpreter.execute();

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


    if(every_10_ms) {
    float battery_voltage = battery_sensor.v_bat;

    update_msg.ms = millis();
    update_msg.us = micros();

    update_msg.rx_str = str.readMicroseconds();
    update_msg.rx_esc = esc.readMicroseconds();

    update_msg.ax = mpu9150.ax;
    update_msg.ay = mpu9150.ay;
    update_msg.az = mpu9150.az;

    noInterrupts();
    update_msg.spur_us = motor.last_change_us;
    update_msg.spur_odo = motor.odometer;
    interrupts();

    update_msg.mode = modes.current_task->name;

    noInterrupts();
    update_msg.odo_fl_a = odo_fl.odometer_a;
    update_msg.odo_fl_a_us = odo_fl.last_odometer_a_us;
    update_msg.odo_fl_b = odo_fl.odometer_b;
    update_msg.odo_fl_b_us = odo_fl.last_odometer_b_us;
    update_msg.odo_fl_ab_us = odo_fl.odometer_ab_us;
    interrupts();

    noInterrupts();
    update_msg.odo_fr_a = odo_fr.odometer_a;
    update_msg.odo_fr_a_us = odo_fr.last_odometer_a_us;
    update_msg.odo_fr_b = odo_fr.odometer_b;
    update_msg.odo_fr_b_us = odo_fr.last_odometer_b_us;
    update_msg.odo_fr_ab_us = odo_fr.odometer_ab_us;
    interrupts();

    update_msg.v_bat = battery_voltage;

    update_msg.mpu_deg_yaw = mpu9150.heading();
    update_msg.mpu_deg_pitch = mpu9150.pitch * 180. / M_PI;
    update_msg.mpu_deg_roll = mpu9150.roll * 180. / M_PI;

    update_msg.mpu_deg_f = mpu9150.temperature /340.0 + 35.0;

    // hack: car doesn't have kill switch yet
    update_msg.go = true;

    car_update.publish(&update_msg);
    nh.spinOnce();


  }
}
