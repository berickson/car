#include "Arduino.h"

const int pin_motor_a = 24;
const int pin_motor_b = 25;
const int pin_motor_c = 26;
const int pin_motor_temp = A13;

const int pin_odo_fl_a = 0;
const int pin_odo_fl_b = 1;
const int pin_odo_fr_a = 3;
const int pin_odo_fr_b = 2;

const int pin_str = 8;
const int pin_esc = 9;
const int pin_esc_aux = 10;
const int pin_rx_str = 11;
const int pin_rx_esc = 12;

const int pin_mpu_interrupt = 17;

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
// globals

QuadratureEncoder odo_fl(pin_odo_fl_a, pin_odo_fl_b);
QuadratureEncoder odo_fr(pin_odo_fr_a, pin_odo_fr_b);

PwmInput rx_str;
PwmInput rx_esc;

Servo2 str;
Servo2 esc;

MotorEncoder motor(pin_motor_a, pin_motor_b, pin_motor_c);

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


void setup() {
  // put your setup code here, to run once:
  rx_str.attach(pin_rx_str);
  rx_esc.attach(pin_rx_esc);
  str.attach(pin_str);
  esc.attach(pin_esc);

  attachInterrupt(pin_motor_a, motor_a_changed, CHANGE);
  attachInterrupt(pin_motor_b, motor_b_changed, CHANGE);
  attachInterrupt(pin_motor_c, motor_c_changed, CHANGE);

  attachInterrupt(pin_odo_fl_a, odo_fl_a_changed, CHANGE);
  attachInterrupt(pin_odo_fl_b, odo_fl_b_changed, CHANGE);
  attachInterrupt(pin_odo_fr_a, odo_fr_a_changed, CHANGE);
  attachInterrupt(pin_odo_fr_b, odo_fr_b_changed, CHANGE);


  attachInterrupt(pin_rx_str, rx_str_handler, CHANGE);
  attachInterrupt(pin_rx_esc, rx_esc_handler, CHANGE);

  Serial.println("starting wire");
  Wire.begin();

  Serial.println("starting mpu");

  mpu9150.enable_interrupts(pin_mpu_interrupt);
  log(LOG_INFO, "Interrupts enabled for mpu9150");
  mpu9150.setup();
  log(LOG_INFO, "MPU9150 setup complete");
  mpu9150.ax_bias = 0; // 7724.52;
  mpu9150.ay_bias = 0; // -1458.47;
  mpu9150.az_bias = 0; // 715.62;
  mpu9150.rest_a_mag = 0; // 7893.51;
  mpu9150.zero_adjust = Quaternion(0.0, 0.0, 0.0, 1);// Quaternion(-0.07, 0.67, -0.07, 0.73);
  // was ((-13.823402+4.9) / (1000 * 60 * 60)) * PI/180;
  mpu9150.yaw_slope_rads_per_ms  = (2.7 / (10 * 60 * 1000)) * PI / 180;
  mpu9150.yaw_actual_per_raw = 1; //(3600. / (3600 - 29.0 )); //1.0; // (360.*10.)/(360.*10.-328);// 1.00; // 1.004826221;

  mpu9150.zero_heading();
  Serial.println("mpu setup complete");  

}
unsigned long loop_time_ms = 0;
unsigned long last_loop_time_ms = 0;

void loop() {
  last_loop_time_ms = loop_time_ms;
  loop_time_ms = millis();

  mpu9150.execute();

  if (every_n_ms(last_loop_time_ms, loop_time_ms, 1)) {
    if(rx_str.pulse_us() > 0 && rx_esc.pulse_us() > 0) {
      str.writeMicroseconds(rx_str.pulse_us());
      esc.writeMicroseconds(rx_esc.pulse_us());
    } else {
      str.writeMicroseconds(1500);
      esc.writeMicroseconds(1500);
    }
  }

  if (every_n_ms(last_loop_time_ms, loop_time_ms, 500)) {
    Serial.print((String) "a: " + motor.a_count + " b: " + motor.b_count + " c: " + motor.c_count + " odo:" + motor.odometer + " temp: " + analogRead(pin_motor_temp));
    Serial.print((String) " heading: " + mpu9150.heading());
    Serial.print((String) " fl: (" + odo_fl.odometer_a + ","+odo_fl.odometer_b+")");
    Serial.print((String) " fr: (" + odo_fr.odometer_a + ","+odo_fr.odometer_b+")");
    Serial.println();
  }
}
