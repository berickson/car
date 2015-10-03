#include "ManualMode.h"
#include "Arduino.h"
#include "Servo.h"
#include "PwmInput.h"

extern Servo speed;
extern Servo steering;
extern PwmInput rx_steer;
extern PwmInput rx_speed;

void ManualMode::begin() {
}

void ManualMode::end() {
    steering.writeMicroseconds(1500);
    speed.writeMicroseconds(1500);
}

void ManualMode::execute() {
  if(rx_steer.pulse_us() > 0 && rx_speed.pulse_us() > 0) {
    steering.writeMicroseconds(rx_steer.pulse_us());
    speed.writeMicroseconds(rx_speed.pulse_us());
  } else {
    steering.writeMicroseconds(1500);
    speed.writeMicroseconds(1500);
  }
}

