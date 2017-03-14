#include "speedometer.h"
#include "kalman.h"
#include <algorithm>

Speedometer::Speedometer()  {
}

int Speedometer::get_ticks() const {
  return last_ticks;
}

double Speedometer::get_velocity() const {
  return velocity;
}

double Speedometer::get_smooth_velocity() const
{
  return kalman_v.mean;
}

double Speedometer::get_meters_travelled() const {
  return meters_travelled;
}

double Speedometer::update_from_sensor(unsigned int clock_us, int odo_a, unsigned int a_us, int odo_b, unsigned int b_us, int ab_us, float ax) {
  unsigned tick_us = std::max(a_us,b_us);
  int ticks = odo_a + odo_b;
  double meters_moved = 0.0;
  double elapsed_seconds = (tick_us - last_tick_us) / 1000000.;
  if (elapsed_seconds > 0) {
    meters_moved = meters_per_tick * (ticks - last_ticks);

    velocity = meters_moved / elapsed_seconds;
    last_tick_us = tick_us;
    last_ticks = ticks;
  } else {
    // no tick this time, how long has it been?
    elapsed_seconds= ( clock_us - last_tick_us) / 1000000.;
    if (elapsed_seconds > 0.1){
      // it's been a long time, let's call velocity zero
      velocity = 0.0;
    } else {
      // we've had a tick recently, fastest possible speed is when a tick is about to happen
      // do nothing unless smaller than previously certain velocity
      double  max_possible = meters_per_tick / elapsed_seconds;
      if(max_possible < fabs(velocity)){
        if(velocity > 0)
          velocity = max_possible;
        else
          velocity = -max_possible;
      }
    }
  }
  meters_travelled += meters_moved;
  kalman_v.update(elapsed_seconds*ax,elapsed_seconds*elapsed_seconds);
  kalman_v.measure(velocity,0.2);
  return meters_moved;
}
