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

  double w; // width of front wheelbase
  double l;
  double x; // x and y are at center of rear wheels
  double y;
  double heading;

  Ackerman(double front_wheelbase_width = 1.0, double wheelbase_length = 1.0, double x=0., double y = 0, double radians = 0);

  inline Point front_left_position() {
    Point p;
    p.x = x + l * cos(heading) - sin(heading) * w/2;
    p.y = y + l * sin(heading) - cos(heading) * w/2;
    return p;
  }

  inline Point front_position() {
    Point p;
    p.x = x + l * cos(heading);
    p.y = y + l * sin(heading) ;
    return p;
  }

  inline Point rear_position() {
    Point p;
    p.x = x;
    p.y = y;
    return p;
  }


  void move_right_wheel(Angle outside_wheel_angle, double wheel_distance, double new_heading = NAN);
  Arc arc_to_relative_location(double x,double y);
  string to_string();
};

// calculates an arc where the front wheel will travel to
// point x ahead and point y t left
void test_arc_to_relative_location(double l, double x, double y);
void arc_to_relative_location_tests();
void move_left_wheel_tests();

void test_ackerman();

#endif // ACKERMAN_H
