#ifndef DRIVER_H
#define DRIVER_H
#include "car.h"
#include "car_ui.h"
#include "route.h"
#include "run_settings.h"





class Driver
{
public:
  Driver(Car & car, CarUI & ui, RunSettings & settings);
  Car & car;
  CarUI & ui;
  RunSettings & settings;

  void drive_route(Route & route);
  Angle curvature_by_look_ahead(Route & route, double ahead);
  Angle steering_angle_by_cte(Route & route);
private:
  bool rear_slipping();
  int esc_for_max_decel();
  int esc_for_velocity(double goal_velocity, double goal_accel);
};


void test_driver();

#endif // DRIVER_H
