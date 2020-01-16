#ifndef DYNAMICS_H
#define DYNAMICS_H

#include <chrono>
#include <string>
#include <sstream>
#include "geometry.h"
#include "work-queue.h"



using namespace std;
using namespace std::chrono;

struct Dynamics {
public:
  Dynamics();
  static bool from_log_string(Dynamics &d, const StampedString &s);
  string display_string();

  system_clock::time_point datetime;
  int str; // steering
  int esc; // esc

  // acceleration
  double ax;
  double ay;
  double az;
  
  unsigned int spur_last_us;
  int spur_odo;
  char control_mode;

  int odo_fl_a;
  unsigned int odo_fl_a_us;
  int odo_fl_b;
  unsigned int odo_fl_b_us;
  
  int odo_fr_a;
  unsigned int odo_fr_a_us;
  int odo_fr_b;
  unsigned int odo_fr_b_us;
  
  int odo_bl_a;
  unsigned int odo_bl_a_us;
  int odo_bl_b;
  unsigned int odo_bl_b_us;
  
  int odo_br_a;
  unsigned int odo_br_a_us;
  int odo_br_b;
  unsigned int odo_br_b_us;
  
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
