#ifndef DRIVER_H
#define DRIVER_H
#include "car.h"
#include "car_ui.h"
#include "route.h"



class Driver
{
public:
  Driver(Car& car, CarUI ui);
  Car & car;
  CarUI & ui;
  double k_smooth = 0.4;
  double t_ahead = 0.2;
  double d_ahead = 0.05;
  void play_route(Route & route);
  Angle steering_angle_by_look_ahead(Route & route);
  Angle steering_angle_by_cte(Route & route);
};


#endif // DRIVER_H
