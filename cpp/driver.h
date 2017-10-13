#ifndef DRIVER_H
#define DRIVER_H
#include "car.h"
#include "car_ui.h"
#include "route.h"
#include "run_settings.h"
#include "pid.h"
#include "stereo_camera.h"




class Driver
{
public:
  Driver(Car & car, RunSettings & settings);
  Car & car;
  RunSettings & settings;

  void drive_route(Route & route, StereoCamera &camera);
  Angle required_turn_curvature_by_look_ahead(Route & route, double ahead);
  Angle steering_angle_by_cte(Route & route);
  bool check_for_crash();
private:
  void avoid_barrels(StereoCamera &camera);
  bool route_complete = false;
  bool recovering_from_crash = false;
  struct Checkpoint {
    Point position;
    unsigned int ms;
    bool valid = false;
  } crash_checkpoint, current_crash, previous_crash;
  Point last_crash_correction;

  string mode;

  bool rear_slipping();
  int esc_for_max_decel();
  int esc_for_velocity(PID &velocity_pid, double goal_velocity, double goal_accel);
  void continue_along_route(Route& route, StereoCamera & camera);
  bool continue_to_stop(Route& route, StereoCamera & camera);
  void set_evasive_actions_for_crash(Route& route);
};


void test_driver();

#endif // DRIVER_H
