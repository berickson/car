#ifndef SPEEDOMETER_H
#define SPEEDOMETER_H

#include "math.h"

class Speedometer
{
public:
  Speedometer();

  double meters_per_tick = NAN; // set in constructor
  int last_ticks = 0;
  unsigned int last_tick_us = 0;
  double velocity = 0.0;
  double meters_travelled = 0.0;

  int get_ticks() const;
  double get_velocity() const;

  double get_meters_travelled() const;


  // updates internal state and returns meters just moved
  double update_from_sensor(unsigned int clock_us, unsigned int tick_us, int ticks);
};

#endif // SPEEDOMETER_H
