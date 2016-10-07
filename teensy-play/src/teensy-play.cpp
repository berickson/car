#include "Arduino.h"
#include "helper_3dmath.h"

#include <MPU9150_9Axis_MotionApps41.h>

#include "Mpu9150.h"

const uint8_t pin_led = 13;
Mpu9150 mpu9150;
void setup(){
  Serial.begin(250000);
  Wire.begin();
  TRACE_MPU = false;
  delay(10);
  Serial.println("inside setup");
  pinMode(pin_led, OUTPUT);
  Serial.println("inside setup");
  digitalWrite(pin_led,1);
  delay(10);
  Serial.println("inside setup");
  mpu9150.setup();
}

int loop_count = 0;
void loop() {
  ++loop_count;
  //Serial.println("inside loop");
  //
  mpu9150.execute();
  if(loop_count%1000 == 0) {
    mpu9150.log_status();
    Serial.println(loop_count);
    Serial.println(mpu9150.heading());
  }
  //
  digitalWrite(pin_led,micros()%1000000<50000);
  //delay(1000);

  //delay(100);
}
