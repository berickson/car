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


// based on http://stackoverflow.com/a/4436915/383967, modified
Quaternion quaternion_from_axis_angle(const double &xx, const double &yy, const double &zz, const double &a)
{
    // Here we calculate the sin( theta / 2) once for optimization
    double factor = sin( a / 2.0 );

    // Calculate the x, y and z of the quaternion
    double x = xx * factor;
    double y = yy * factor;
    double z = zz * factor;

    // Calcualte the w value by cos( theta / 2 )
    double w = cos( a / 2.0 );
    Quaternion q = Quaternion(w, x, y, z);
    q.normalize();

    return q;
}

void loop() {
  int command = 0;
  while(Serial.available()) {
    command = Serial.read();
    if(command) {
      switch (command) {
      case 'g': {
        Serial.println("this will calibrate rest position based on gravity alone");
        mpu9150.set_zero_orientation(mpu9150.qraw);
      }
      break;
      case 't': {
        Serial.println("this will calibrate forward based on tilt and gravity");
        Serial.print("Based on gravity, the offset should be ");
        double dx = mpu9150.gravity.x;
        double dy = mpu9150.gravity.y;
        double theta = atan2(dy,dx);
        Serial.print("dx: ");
        Serial.print(dx);
        Serial.print(" dy: ");
        Serial.print(dy);
        Serial.print(" atan: ");
        Serial.print(theta * 180. / PI);
        Serial.println();

        Quaternion q = quaternion_from_axis_angle(0,0,1,PI-theta );
        mpu9150.zero_adjust = q.getProduct(mpu9150.zero_adjust);
      }
        break;
      default: {
        break;
      }
      }
    }
  }
  loop_tracker.execute();

  //Serial.println("inside loop");
  //
  mpu9150.execute();
  if(loop_tracker.every_n_ms(1000)) {
    Serial.print((String) (millis()/1000.) +  " YPR[" + mpu9150.heading() + "," + mpu9150.pitch * 180./PI + "," + mpu9150.roll * 180./PI + "]");
    Serial.print((String) " qraw[" + mpu9150.qraw.w+ "," + mpu9150.qraw.x+ ","+ mpu9150.qraw.y+ ","+ mpu9150.qraw.z + "]");
    Serial.print((String) " q[" + mpu9150.q.w+ "," + mpu9150.q.x+ ","+ mpu9150.q.y+ ","+ mpu9150.q.z + "]");
    Serial.print((String) " g[" + mpu9150.gravity.x+ ", " + mpu9150.gravity.y+ ", "+ mpu9150.gravity.z + "]");
    Serial.println((String) " a[" + mpu9150.ax+ ", " + mpu9150.ay+ ", "+ mpu9150.az + "]");

  }
  //
  digitalWrite(pin_led,micros()%1000000<50000);
  //delay(1000);

  //delay(100);
}
