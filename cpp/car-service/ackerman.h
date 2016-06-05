#ifndef ACKERMAN_H
#define ACKERMAN_H

#include "ackerman.h"

#include "geometry.h"

#include <math.h>
#include <string>
#include <sstream>
#include <iostream>


using namespace std;

class Ackerman {
public:
  struct Arc {
    double steer_radians;
    double arc_radians;
    double r;
    double arc_len;
  };

  double _w;
  double _l;
  double _x;
  double _y;
  double _heading;

  Ackerman(double front_wheelbase_width = 1.0, double wheelbase_length = 1.0, double x=0., double y = 0, double radians = 0);

  inline Point front_left_position() {
    Point p;
    p.x = _x + _l * cos(_heading) - sin(_heading) * _w/2;
    p.y = _y + _l * sin(_heading) - cos(_heading) * _w/2;
    return p;
  }

  void move_left_wheel(double outside_wheel_angle, double wheel_distance, double new_heading = NAN);
  Arc arc_to_relative_location(double x,double y);
  string to_string();
};

// calculates an arc where the front wheel will travel to
// point x ahead and point y t left
void test_arc_to_relative_location(double l, double x, double y);
void arc_to_relative_location_tests();
void move_left_wheel_tests();

#endif // ACKERMAN_H
