#pragma once

class Mpu9150;

class CircleMode {
public:
  double last_angle;
  double degrees_turned = 0;
  Mpu9150 * mpu;
  bool done = false;

  bool is_done();
  void init(Mpu9150 * _mpu);
  void end();
  void execute();
};
