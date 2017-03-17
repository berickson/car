#include "speedometer.h"
#include "kalman.h"
#include <algorithm>

Speedometer::Speedometer()  {
}

int Speedometer::get_ticks() const {
  return last_odo_a + last_odo_b;
}

double Speedometer::get_velocity() const {
  return velocity;
}

double Speedometer::get_smooth_velocity() const {
  return kalman_v.mean;
}

double Speedometer::get_smooth_acceleration() const {
  return this->smooth_a;
}

double Speedometer::get_meters_travelled() const {
  return meters_travelled;
}

double Speedometer::update_from_sensor(unsigned int clock_us, int odo_a, unsigned int a_us, int odo_b, unsigned int b_us, int ab_us, float ax) {
  auto dt = ab_us - last_ab_us;
  auto ds = 0;


  // calculate velocity based int ab time in hall sensor (1mm gap)
  // this is more accurate that using distance measure below for velocity

  // if forward use a to be time, reverse use b to a time
  if (odo_b > last_odo_b) {
    ds = odo_b - last_odo_b; // forward
  } else if (odo_a < last_odo_a) {
    ds = odo_a - last_odo_a;
  } else {
    ds = 0;
  }
  if (ds != 0 ) {
    velocity = float(ds)/dt*1600;  // 1,000,000 nanos / sec * 1m  /1000 mm * 1.6 wheel ratioinner / outer wheel ratio
  }

  unsigned tick_us = std::max(a_us,b_us);
  unsigned last_tick_us = std::max(last_a_us, last_b_us);
  double meters_moved = ((odo_a - last_odo_a) + (odo_b - last_odo_b))*meters_per_tick;
  double elapsed_seconds = (tick_us - last_tick_us) / 1000000.;
  if (elapsed_seconds == 0) {
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
  double last_v = kalman_v.mean;
  meters_travelled += meters_moved;
  kalman_v.update(elapsed_seconds*ax,elapsed_seconds*elapsed_seconds);
  kalman_v.measure(velocity,0.01);
  if(elapsed_seconds > 0) {
    smooth_a = kalman_v.mean - last_v / elapsed_seconds;
  }

  last_odo_a = odo_a;
  last_odo_b = odo_b;
  last_a_us  = a_us;
  last_b_us  = b_us;
  last_ab_us = ab_us;

  return meters_moved;
}
