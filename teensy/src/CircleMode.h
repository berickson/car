#pragma once
#include "Pid.h"
#include "Task.h"
class Mpu9150;

class CircleMode : public Task {
public:
  Pid pid;
  double last_angle;
  double degrees_turned = 0;
  Mpu9150 * mpu;
  bool done = false;
  unsigned long start_millis;

  bool is_done();
  void init(Mpu9150 * _mpu);
  virtual void begin();
  virtual void end();
  virtual void execute();
};
