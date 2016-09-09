#include "speedometer.h"


Speedometer::Speedometer()  {
}

unsigned int Speedometer::get_ticks() const {
  return last_ticks;
}

double Speedometer::get_velocity() const {
  return velocity;
}

double Speedometer::get_meters_travelled() const {
  return meters_travelled;
}

double Speedometer::update_from_sensor(unsigned int clock_us, unsigned int tick_us, int ticks) {
  double meters_moved = 0.0;
  if (tick_us != last_tick_us) {
    double elapsed_seconds = (tick_us - last_tick_us) / 1000000.;
    meters_moved = meters_per_tick * (ticks - last_ticks);

    velocity = meters_moved / elapsed_seconds;
    last_tick_us = tick_us;
    last_ticks = ticks;
  } else {
    // no tick this time, how long has it been?
    double seconds_since_tick = ( clock_us - last_tick_us) / 1000000.;
    if (seconds_since_tick > 0.1){
      // it's been a long time, let's call velocity zero
      velocity = 0.0;
    } else {
      // we've had a tick recently, fastest possible speed is when a tick is about to happen
      // do nothing unless smaller than previously certain velocity
      double  max_possible = meters_per_tick / seconds_since_tick;
      if(max_possible < fabs(velocity)){
        if(velocity > 0)
          velocity = max_possible;
        else
          velocity = -max_possible;
      }
    }
  }
  meters_travelled += meters_moved;
  return meters_moved;
}
