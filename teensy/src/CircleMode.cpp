#include "Arduino.h"
#include "CircleMode.h"
#include "Esc.h"
#include "Mpu9150.h"

extern Esc esc;
extern Servo steering;
extern Servo speed;

bool CircleMode::is_done() {
  return done;
}

void CircleMode::init(Mpu9150 * _mpu) {
  mpu = _mpu;
}

void CircleMode::begin() {
  last_angle = mpu->ground_angle();
  degrees_turned = 0;

  // assumes power is -1 to 1 range
  // assumes per second (not millis)
  pid.set_sp(90);
  pid.set_pv(abs(degrees_turned), 0.0);
  pid.kp = 1/25.; // full power until 25 degrees
  pid.ki = 0.0;
  pid.kd = 0.0;

  pid.set_min_max_output(-1,1);
  start_millis = millis();
  done = false;
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

  const bool use_pid = true;
  if(use_pid) {
      pid.set_pv(abs(degrees_turned),(millis()-start_millis)/1000.);
      double v = pid.get_output();  // will be in range (-1,1)
      const double max_speed_ms = 60;
      speed.writeMicroseconds(1500+(v*max_speed_ms));
      steering.writeMicroseconds(1100); // turn left todo: make steer commands
  } else {
      if(abs(degrees_turned) < 90) {
        steering.writeMicroseconds(1100); // turn left todo: make steer commands
        esc.set_command(Esc::speed_forward);
      } else {
        end();
      }
  }
}

void CircleMode::end() {
    speed.writeMicroseconds(1500);
    esc.set_command(Esc::speed_neutral);
    steering.writeMicroseconds(1500); // look straight ahead
    done = true;
    Serial.println("circle complete"); 
}
