#include "Arduino.h"
#include "helper_3dmath.h"

#include <MPU9150_9Axis_MotionApps41.h>

#include "Mpu9150.h"


const uint8_t pin_led = 13;
Mpu9150 mpu9150;
void setup(){
  Serial.begin(250000);
  delay(1000);
  Wire.begin();
  TRACE_MPU = false;
  delay(10);
  Serial.println("inside setup");
  pinMode(pin_led, OUTPUT);
  Serial.println("inside setup2");
  digitalWrite(pin_led,1);
  Serial.println("inside setup3");
  //delay(10);
  Serial.println("setting orientatino");
  //[0.70, -0.18, -0.67, -0.17]
  //Quaternion zero(0.674316,-0.0788574,-0.731384,0.0640259);
  Quaternion zero(0.72, -0.00, -0.70,  0.01);
  mpu9150.set_zero_orientation(zero);
  Serial.println("inside setup");
  mpu9150.setup();
  Serial.println("setup complete");

}

struct LoopTracker {
  unsigned long last_loop_ms = 0;
  unsigned long loop_ms = 0;
  unsigned long loop_count = 0;
  // todo: run execute once only at the top of each loop
  void execute() {
    ++loop_count;
    last_loop_ms = loop_ms;
    loop_ms = millis();
  }
  // returns true if loop time just passed through n ms boundary
  bool every_n_ms(unsigned long ms) {
    return (last_loop_ms % ms) + (loop_ms - last_loop_ms) >= ms;
  }
} loop_tracker;


int mpu_reporting_ms = 1000;

void loop() {
  int command = 0;
  while(Serial.available()) {
    command = Serial.read();
    if(command) {
      switch (command) {
      case 'g':
        mpu9150.calibrate_as_horizontal();
        break;

      case 't':
        mpu9150.calibrate_nose_up();
        break;

      case 'r':
        mpu9150.start_calibrate_at_rest(1,60);
        break;

      case 'm':
        mpu_reporting_ms = 60*1000;
        Serial.println("set to print MPU status on every minute");
        break;

      case 's':
        mpu_reporting_ms = 1000;
        Serial.println("set to print MPU status on every second");
        break;

      default:
        break;
      }
    }
  }
  loop_tracker.execute();

  mpu9150.execute();
  if(loop_tracker.every_n_ms(mpu_reporting_ms)) {
    Serial.print((String) (millis()/1000) +  " ypr[" + mpu9150.heading() + "," + mpu9150.pitch * 180./PI + "," + mpu9150.roll * 180./PI + "]");
    Serial.print((String) " z[" + mpu9150.zero_adjust.w+ "," + mpu9150.zero_adjust.x+ ","+ mpu9150.zero_adjust.y+ ","+ mpu9150.zero_adjust.z + "]");
    Serial.print((String) " qr[" + mpu9150.qraw.w+ "," + mpu9150.qraw.x+ ","+ mpu9150.qraw.y+ ","+ mpu9150.qraw.z + "]");
    Serial.print((String) " ar[" + mpu9150.araw.x+ "," + mpu9150.araw.y+ ","+ mpu9150.araw.z + "]");
    Serial.print((String) " q[" + mpu9150.q.w+ "," + mpu9150.q.x+ ","+ mpu9150.q.y+ ","+ mpu9150.q.z + "]");
    Serial.print((String) " g[" + mpu9150.gravity.x+ "," + mpu9150.gravity.y+ ","+ mpu9150.gravity.z + "]");
    Serial.print((String) " a[" + mpu9150.ax+ "," + mpu9150.ay+ ","+ mpu9150.az + "]");
    Serial.println();

  }
  //
  digitalWrite(pin_led,micros()%1000000<50000);
}
