#include "speedometer.h"
#include "kalman.h"
#include <algorithm>
#include <sstream>
#include "json.hpp"
using namespace std;

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
  return kalman_a.mean;
}

double Speedometer::get_meters_travelled() const {
  return meters_travelled;
}

nlohmann::json Speedometer::get_json_state() const
{
  nlohmann::json j;

  j["v"] = get_velocity();
  j["v_smooth"] = get_smooth_velocity();
  j["a_smooth"] = get_smooth_acceleration();
  j["meters"] = get_meters_travelled();
  j["ticks"] = get_ticks();
  return j;
}

double Speedometer::update_from_sensor(unsigned int clock_us, int odo_a, unsigned int a_us, int odo_b, unsigned int b_us, int ab_us) {
  double last_v = velocity;

  if (a_us > last_a_us) {
    v_a =  (odo_a-last_odo_a)*2*meters_per_tick / (a_us - last_a_us) *1E6;
  } 
  if(odo_b != last_odo_b) {
    v_b =  (odo_b-last_odo_b)*2*meters_per_tick / (b_us - last_b_us) *1E6;
  }
  velocity = (v_a + v_b) / 2.;


#if 0
  // calculate velocity based int ab time in hall sensor (1mm gap)
  // this is more accuratel that using distance measure below for velocity
  auto dt = ab_us - last_ab_us;
  auto ds = 0;

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
#endif
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
  meters_travelled += meters_moved;
  if(last_clock_us > 0) {
    auto dt = (clock_us - last_clock_us) * 1E-6;
    kalman_v.predict(0,100.0 *dt*dt); // sigma = 10m/s^2 * dt, variance = sigma^2 = 100 * dt^2
    kalman_a.predict(0,900.0*dt*dt);  // sigma = 30m/s^3 * dt, variance = 900 m/s^2 * dt^2
  }
  kalman_v.update(velocity,0.01);
  if(elapsed_seconds > 0) {
    auto a = (velocity - last_v) / elapsed_seconds;
    kalman_a.update(a,1.0*1.0);
  }

  last_odo_a = odo_a;
  last_odo_b = odo_b;
  last_a_us  = a_us;
  last_b_us  = b_us;
  last_ab_us = ab_us;
  last_clock_us = clock_us;

  return meters_moved;
}
