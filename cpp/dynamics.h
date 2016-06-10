#ifndef DYNAMICS_H
#define DYNAMICS_H

#include <chrono>
#include <string>
#include <sstream>
#include "geometry.h"



using namespace std;
using namespace std::chrono;

struct Dynamics {
public:
  Dynamics();
  static bool from_log_string(Dynamics &d, string &s);
  string to_string();

  system_clock::time_point datetime;
  int str; // steering
  int esc; // esc

  // acceleration
  double ax;
  double ay;
  double az;

  int spur_delta_us;

  int spur_last_us;
  int spur_odo;
  int ping_millimeters;

  int odometer_front_left;
  int odometer_front_left_last_us;
  int odometer_front_right;
  int odometer_front_right_last_us;



  int odometer_back_left;
  int odometer_back_left_last_us;
  int odometer_back_right;
  int odometer_back_right_last_us;
  int ms;
  int us;
  Angle yaw;
  Angle pitch;
  Angle roll;
  double battery_voltage;
};

void test_dynamics();

#endif // DYNAMICS_H
