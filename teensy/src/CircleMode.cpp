#include "Arduino.h"
#include "CircleMode.h"
#include "Esc.h"
#include "Mpu9150.h"
#include "Logger.h"

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
  Serial.println("begin of circle mode");
  last_angle = mpu->ground_angle();
  degrees_turned = 0;

  // assumes power is -1 to 1 range
  // assumes per second (not millis)
  pid.reset();
  pid.set_sp(angle_to_turn);
  log(LOG_INFO, "angle_to_turn: " + angle_to_turn);
  pid.set_pv(degrees_turned, 0.0);
  pid.kp = 1/25.; // full power until 25 degrees
  pid.ki = 0.0;
  pid.kd = 1./90; // 90 degrees per second

  pid.set_min_max_output(-1,1);
  start_millis = millis();
  done = false;
}

void CircleMode::execute() {
  if(done) {
    return;
  }
  if(abs(degrees_turned-angle_to_turn) < 1) { // todo: maybe put some logic about slow velocity
    end();
  }
  log(LOG_TRACE, "circle has turned " + degrees_turned);
  double ground_angle = mpu->ground_angle();
  double angle_diff = last_angle-ground_angle;
  if(abs(angle_diff) > 70){
    last_angle = ground_angle; // cheating low tech way to avoid wrap around
    return;
  }
  degrees_turned += angle_diff;
  last_angle = ground_angle;

  pid.set_pv(degrees_turned,(millis()-start_millis)/1000.);
  double v = pid.get_output();  // will be in range (-1,1)
  esc.set_velocity(v);
  steering.writeMicroseconds(1100); // turn left todo: make steer commands
}

void CircleMode::end() {
    esc.set_velocity(0);
    steering.writeMicroseconds(1500); // look straight ahead
    done = true;
    Serial.println("circle complete"); 
}
