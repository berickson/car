#ifndef SPEEDOMETER_H
#define SPEEDOMETER_H

#include "math.h"
#include "kalman.h"
#include <string>
#include "json.hpp"

class Speedometer
{
public:
  Speedometer();

  double meters_per_tick = 0; // set by client
  int last_odo_a = 0;
  int last_odo_b = 0;
  unsigned int last_a_us = 0;
  unsigned int last_b_us = 0;
  unsigned int last_ab_us = 0;
  unsigned int last_clock_us = 0;

  double velocity = 0.0;
  double meters_travelled = 0.0;
  double v_a = 0.0;
  double v_b = 0.0;

  int get_ticks() const;
  double get_velocity() const;
  double get_smooth_velocity() const;
  double get_smooth_acceleration() const;
  double get_meters_travelled() const;
  nlohmann::json get_json_state() const;


  // updates internal state and returns meters just moved
  double update_from_sensor(unsigned int clock_us, int odo_a, unsigned int a_us, int odo_b = 0, unsigned int b_us = 0);

  KalmanFilter kalman_v;
  KalmanFilter kalman_a;
};

#endif // SPEEDOMETER_H
