#include "ackerman.h"

#include "geometry.h"

#include <math.h>
#include <string>
#include <sstream>
#include <iostream>


using namespace std;

Ackerman::Ackerman(double front_wheelbase_width, double wheelbase_length, Point front_position, Angle _heading) {
  w = front_wheelbase_width;
  l = wheelbase_length;
  heading = _heading;
  x = front_position.x - l*cos(heading);
  y = front_position.y - l*sin(heading);
}

Point Ackerman::front_left_position() const{
  Point p;
  p.x = x + l * cos(heading) - sin(heading) * w/2;
  p.y = y + l * sin(heading) - cos(heading) * w/2;
  return p;
}

Point Ackerman::front_position() const {
  Point p;
  p.x = x + l * cos(heading);
  p.y = y + l * sin(heading) ;
  return p;
}

Point Ackerman::rear_position() const {
  Point p;
  p.x = x;
  p.y = y;
  return p;
}

string Ackerman::to_string() const {
  stringstream ss;
  ss <<  "Ackerman x:" << x << " y:" << y << " heading: " << heading;
  return ss.str() ;
}

void Ackerman::move_relative_to_heading(Point p) {
  this->x += cos(heading) * p.x + sin(heading) * p.y;
  this->y += sin(heading) * p.x + cos(heading) * p.y;
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
    arc.curvature = 1 / arc.r;

  } else {
    arc.r = 1.0E100;
    arc.steer_radians = 0.0;
    arc.arc_radians = 0.0;
    arc.arc_len = distance(0.0,0.0,x,y);
    arc.curvature = 0.0;
  }

  if(y < 0.) {
    arc.steer_radians = -arc.steer_radians;
    arc.curvature = -arc.curvature;
  }
  return arc;

}

void Ackerman::move_right_wheel(Angle outside_wheel_angle, double wheel_distance, double new_heading){

  // avoid errors for very small angles

  double turn_angle, arc_distance, forward, left;

  if(fabs(outside_wheel_angle.degrees()) < 0.001) {
    turn_angle = 0.;
    arc_distance = wheel_distance;
    forward = arc_distance;
    left = 0.;
  } else {
    // calculate front wheel horizontal offset from center of rear wheels to outside
    double offset = outside_wheel_angle.radians() < 0. ? -w/2. : w/2.;

    // calculate turn radius of center rear of car
    double r_car = l / tan(outside_wheel_angle.radians()) - offset;

    // calculate turn radius of right wheel
    double r_right = length(r_car + offset,l);
    // make radius negative for right turns
    if (outside_wheel_angle.radians() < 0) {
      r_right = -r_right;
    }

    // calculate angle travelled
    turn_angle = wheel_distance / r_right;
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

#include "string-utils.h"

// calculates an arc where the front wheel will travel to
// point x ahead and point y t left
void test_arc_to_relative_location(double l, double x, double y){
  auto car = Ackerman(0, l, Point(0,0)); // zero width makes like a bicycle, -l puts front wheel at (0,0)
  cout << "beginning car fl position:" << car.front_left_position().to_string();

  auto arc = car.arc_to_relative_location(x,y);
  car.move_right_wheel(Angle::radians(arc.steer_radians), arc.arc_len);
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



void test_move_right_wheel(double l, Angle outside_wheel_angle, double distance) {
  auto car = Ackerman(0, l, Point(0, 0)); // zero width makes like a bicycle
  cout << "start fl:" << car.front_left_position().to_string()
    << " l: " << l
    << " outside_wheel_angle: " << outside_wheel_angle.degrees() << " degrees"
    << " distance: " << distance;

  car.move_right_wheel(outside_wheel_angle,distance);

  cout << " final fl:" << car.front_left_position().to_string()
       << endl;

}

void move_right_wheel_tests() {
  test_move_right_wheel(100000,Angle::degrees(45),1);
  test_move_right_wheel(10,Angle::degrees(10),1);
  test_move_right_wheel(10,Angle::degrees(-10),1);

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

void test_ackerman() {
  Ackerman ackerman;
  cout << "front: " << ackerman.front_position().to_string();
  cout << "rear: " << ackerman.rear_position().to_string();

  move_right_wheel_tests();
  //arc_to_relative_location_tests();
}


struct Pose {

  // returns pose x ahead, y to left with incremental angle theta
  Pose relative_pose(double forward, double left, Angle dtheta) const {
    Pose after;
    double h = heading.radians();
    after.x = x + cos(h)*forward - sin(h)*left;
    after.y = y + sin(h)*forward + cos(h)*left;
    after.heading = heading + dtheta;
    after.heading.standardize();
    return after;
  }

  double x = 0;
  double y = 0;
  Angle heading = Angle::degrees(0);
  std::string to_string() const {
    stringstream ss;
    ss <<  "(" << format(x,3,2) << "," << format(y,3,2) << "," << heading.degrees() << "°)";
    return ss.str();
  }
};

class Ackerman2 {
public:
  double w = 1.0; // width of front wheelbase
  double l = 1.0; // from front to back wheels
  const int fl_meters_per_tick = 1;
  const int fr_meters_per_tick = 1;
  const int bl_meters_per_tick = 1;
  const int br_meters_per_tick = 1;

  Pose move_rear_wheels(const Pose before, double d_bl, double d_br) const {
    // special case if both wheels travelled the same distance
    Pose after;
    if(d_bl == d_br) {
      after = before.relative_pose(d_bl,0.,Angle::radians(0));
      return after;
    }
    if(fabs(d_bl) > fabs(d_br)) {
      // different distances, assume going in a big circle (to right for now)
      double r = ((w * d_br) /  (d_br - d_bl)) + (w/2);
      Angle theta = Angle::radians((d_br-d_bl) / w);
      double forward = r*sin(-theta.radians());
      double left = r*(cos(-theta.radians()) - 1);
      after = before.relative_pose(forward,left,theta);
      return after;
    } else {
      // different distances, assume going in a big circle (to left for now)
      double r = ((w * d_bl) /  (d_bl - d_br)) + (w/2);
      Angle theta = Angle::radians((d_br-d_bl) / w);
      double forward = r*sin(theta.radians());
      double left = r*(1-cos(theta.radians()));
      after = before.relative_pose(forward,left,theta);
      return after;
    }
  }
};


Pose test_move_rear_wheels(Ackerman2 car, Pose before, double bl, double br) {
  cout << before.to_string() << " + " << "move_rear_wheels(" << format(bl,3,2) << "," << format(br,3,2) << ")" ;
  Pose after = car.move_rear_wheels(before, bl,br);
  cout << " ->  " << after.to_string() << endl;
  return after;
}

void test_ackerman2() {
  Ackerman2 car;
  Pose pose;
  //pose = test_move_rear_wheels(car, pose,1,1);
  pose = test_move_rear_wheels(car, pose,Angle::degrees(90).radians(),0);
  pose = test_move_rear_wheels(car, pose,Angle::degrees(90).radians(),0);
  pose = test_move_rear_wheels(car, pose,Angle::degrees(90).radians(),0);
  pose = test_move_rear_wheels(car, pose,Angle::degrees(90).radians(),0);
  cout << endl;
  pose = test_move_rear_wheels(car, pose,0,Angle::degrees(90).radians());
  pose = test_move_rear_wheels(car, pose,0,Angle::degrees(90).radians());
  pose = test_move_rear_wheels(car, pose,0,Angle::degrees(90).radians());
  pose = test_move_rear_wheels(car, pose,0,Angle::degrees(90).radians());
  cout << endl;
  pose = test_move_rear_wheels(car, pose,0,Angle::degrees(-90).radians());
  pose = test_move_rear_wheels(car, pose,0,Angle::degrees(-90).radians());
  pose = test_move_rear_wheels(car, pose,0,Angle::degrees(-90).radians());
  pose = test_move_rear_wheels(car, pose,0,Angle::degrees(-90).radians());
  //pose = test_move_rear_wheels(car, pose,Angle::degrees(90).radians(),0);
}

void test_pose() {
  for(int i =0 ; i < 4; i++) {
    Pose pose;
    pose.heading = Angle::degrees(90*i);
    pose = pose.relative_pose(1,0,Angle::degrees(0));
    cout << "angle " << pose.heading.degrees() << " relative x:" << format(pose.x,3,1) << " y:" << format(pose.y,3,1) << endl;
  }
}
