#ifndef SPEEDOMETER_H
#define SPEEDOMETER_H

#include "math.h"
#include "kalman.h"

class Speedometer
{
public:
  Speedometer();

  double meters_per_tick = 0; // set by client
  int last_odo_a = 0;
  int last_odo_b = 0;
  int last_a_us = 0;
  int last_b_us = 0;
  int last_ab_us = 0;
  double velocity = 0.0;
  double meters_travelled = 0.0;

  int get_ticks() const;
  double get_velocity() const;
  double get_smooth_velocity() const;

  double get_meters_travelled() const;


  // updates internal state and returns meters just moved
  double update_from_sensor(unsigned int clock_us, int odo_a, unsigned int a_us, int odo_b, unsigned int b_us, int ab_us, float ax);

  KalmanFilter kalman_v;
};

#endif // SPEEDOMETER_H
