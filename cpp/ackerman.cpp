#include "ackerman.h"

#include "geometry.h"

#include <math.h>
#include <string>
#include <sstream>
#include <iostream>


using namespace std;

Ackerman::Ackerman(double front_wheelbase_width, double wheelbase_length, double x_, double y_, double radians) {
  w = front_wheelbase_width;
  l = wheelbase_length;
  x = x_;
  y = y_;
  heading = radians;
}

string Ackerman::to_string() {
  stringstream ss;
  ss <<  "Ackerman x:" << x << " y:" << y << " heading: " << heading;
  return ss.str() ;
}

  // x is ahead, l is to left, x must be positive
Ackerman::Arc Ackerman::arc_to_relative_location(double x,double y) {
  Arc arc;
  if(fabs(y) > 0.000001) {
    double z=length(l+x/2,x/y*(l+x/2));
    double c=length(x,y)/2;

    arc.r = length(z,c);
    arc.steer_radians = asin(clamp(l/arc.r,-.99,0.99));
    arc.arc_radians = 2*asin(c/arc.r);
    arc.arc_len = arc.r * arc.arc_radians;

  } else {
    arc.r = 1.0E100;
    arc.steer_radians = 0.0;
    arc.arc_radians = 0.0;
    arc.arc_len = distance(0.0,0.0,x,y);
  }

  if(y < 0.)
    arc.steer_radians = -arc.steer_radians;
  return arc;

}

void Ackerman::move_left_wheel(double outside_wheel_angle, double wheel_distance, double new_heading){

  // avoid errors for very small angles
  if(fabs(outside_wheel_angle) < 0.00001)
    outside_wheel_angle = 0;

  double turn_angle, arc_distance, forward, left;

  if(outside_wheel_angle == 0.) {
    turn_angle = 0.;
    arc_distance = wheel_distance;
    forward = arc_distance;
    left = 0.;
  } else {
    // calculate front wheel horizontal offset from center of rear wheels to left wheel
    double offset = outside_wheel_angle < 0. ? -w/2. : w/2.;

    // calculate turn radius of center rear of car
    double r_car = l / tan(outside_wheel_angle) - offset;

    // calculate turn radius of left wheel
    double r_left = length(r_car + offset,l);
    // make radius negative for right turns
    if (outside_wheel_angle < 0) {
      r_left = -r_left;
    }

    // calculate angle travelled
    turn_angle = wheel_distance / r_left;
    forward = r_car * sin(turn_angle);
    left = r_car * (1.-cos(turn_angle));
  }

  // finally, move the car based on left and forward
  x += cos(heading)*forward;
  y += sin(heading)*forward;
  x += sin(heading)*left;
  y += cos(heading)*left;

  heading = isnan(new_heading) ? standardized_radians(heading + turn_angle) : new_heading;
}


// calculates an arc where the front wheel will travel to
// point x ahead and point y t left
void test_arc_to_relative_location(double l, double x, double y){
  auto car = Ackerman(0, l, -l, 0, 0); // zero width makes like a bicycle, -l puts front wheel at (0,0)
  cout << "beginning car fl position:" << car.front_left_position().to_string();

  auto arc = car.arc_to_relative_location(x,y);
  car.move_left_wheel(arc.steer_radians, arc.arc_len);
  cout << " l: " << l
       << " x: " << x
       << " y: " << y
       << " steer degrees: " << degrees(arc.steer_radians)
       << " arc degrees: " << degrees(arc.arc_radians)
       << " turn radius: " << arc.r
       << " arc length: " << arc.arc_len
       << " car fl:: " << car.front_left_position().to_string()
       << endl;
}



void test_move_left_wheel(double l, double outside_wheel_angle, double distance) {
  auto car = Ackerman(0, l, -l, 0, 0); // zero width makes like a bicycle, -l puts front wheel at (0,0)
  cout << "start fl:" << car.front_left_position().to_string()
    << " l: " << l
    << " outside_wheel_angle: " << outside_wheel_angle
    << " distance: " << distance;

  car.move_left_wheel(outside_wheel_angle,distance);

  cout << " final fl:" << car.front_left_position().to_string()
       << endl;

}

void move_left_wheel_tests() {
  test_move_left_wheel(100000,M_PI/4,1);

}



void arc_to_relative_location_tests() {
  test_arc_to_relative_location(0.33655,-0.534768912905,-0.138360394072);

  test_arc_to_relative_location(20,20,20); // (l,x,y)
  test_arc_to_relative_location(20,40,20);
  test_arc_to_relative_location(20,20,0.01);
  test_arc_to_relative_location(20,0,20);
  test_arc_to_relative_location(20,-1,20);


  test_arc_to_relative_location(20,20,20);

  test_arc_to_relative_location(20,20,-20);
  test_arc_to_relative_location(20,40,-20);
  test_arc_to_relative_location(20,20,-0.01);

  test_arc_to_relative_location(20,20,0);
}
