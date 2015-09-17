#include "Arduino.h"
#include "CircleMode.h"
#include "Esc.h"
#include "Mpu9150.h"

extern Esc esc;
extern Servo steering;

bool CircleMode::is_done() {
  return done;
}

void CircleMode::init(Mpu9150 * _mpu) {
  mpu = _mpu;
  last_angle = mpu->ground_angle();
  degrees_turned = 0;
  done = false;
}

void CircleMode::end() {
  esc.set_command(Esc::speed_neutral);
  steering.writeMicroseconds(1500); // look straight ahead
}

void CircleMode::execute() {
//    Serial.print("circle has turned");
//    Serial.println(degrees_turned);
  double ground_angle = mpu->ground_angle();
  double angle_diff = last_angle-ground_angle;
  if(abs(angle_diff) > 70){
    last_angle = ground_angle; // cheating low tech way to avoid wrap around
    return;
  }
  degrees_turned += angle_diff;
  last_angle = ground_angle;
  if(abs(degrees_turned) < 90) {
    steering.writeMicroseconds(1900); // turn left todo: make steer commands
    esc.set_command(Esc::speed_forward);
  } else {
    esc.set_command(Esc::speed_neutral);
    steering.writeMicroseconds(1500); // look straight ahead
    done = true;
    Serial.println("circle complete");
  }
}
