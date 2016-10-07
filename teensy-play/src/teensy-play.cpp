#include "Arduino.h"
#include "helper_3dmath.h"

#include <MPU9150_9Axis_MotionApps41.h>

#include "Mpu9150.h"

const uint8_t pin_led = 13;
Mpu9150 mpu9150;
void setup(){
  Serial.begin(9600);
  Serial.println("inside setup");
  pinMode(pin_led, OUTPUT);
  digitalWrite(pin_led,1);
  //mpu9150.setup();
}

void loop() {
  //mpu9150.execute();
  Serial.print(mpu9150.yaw);
  Serial.println('a');
  digitalWrite(pin_led,micros()%1000000<50000);
  //delay(1000);
}
