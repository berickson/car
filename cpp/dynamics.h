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

  unsigned int spur_delta_us;

  unsigned int spur_last_us;
  int spur_odo;
  char control_mode;

  int odometer_front_left_a;
  unsigned int odometer_front_left_a_us;
  int odometer_front_left_b;
  unsigned int odometer_front_left_b_us;
  int odometer_front_left_ab_us;

  int odometer_front_right_a;
  unsigned int odometer_front_right_a_us;
  int odometer_front_right_b;
  unsigned int odometer_front_right_b_us;
  int odometer_front_right_ab_us;

  int odometer_back_left_a;
  unsigned int odometer_back_left_a_us;
  int odometer_back_left_b;
  unsigned int odometer_back_left_b_us;
  int odometer_back_left_ab_us;

  int odometer_back_right_a;
  unsigned int odometer_back_right_a_us;
  int odometer_back_right_b;
  unsigned int odometer_back_right_b_us;
  int odometer_back_right_ab_us;

  unsigned int ms;
  unsigned int us;
  Angle yaw;
  Angle pitch;
  Angle roll;
  double battery_voltage;
  int calibration_status;
  bool go;
  double mpu_temperature = NAN;

  static std::string csv_field_headers();
  std::string csv_fields();
};

void test_dynamics();

#endif // DYNAMICS_H
