#ifndef DRIVER_H
#define DRIVER_H
#include "car.h"
#include "car_ui.h"
#include "route.h"
#include "run_settings.h"



class Driver
{
public:
  Driver(Car& car, CarUI ui, RunSettings settings);
  Car & car;
  CarUI & ui;
  double k_smooth = 0.4;
  double t_ahead = 0.2;
  double d_ahead = 0.05;
  double k_p = 30.;
  double k_d = 1.;
  void drive_route(Route & route);
  Angle curvature_by_look_ahead(Route & route, double ahead);
  Angle steering_angle_by_cte(Route & route);
};


void test_driver();

#endif // DRIVER_H
