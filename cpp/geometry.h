#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "math.h"
#include <string>
#include <vector>
#include <sstream>


using namespace std;

struct Point {
  Point(double x=0, double y=0) :
    x(x),y(y){}
  double x;
  double y;
  inline string to_string() const {
    stringstream ss;
    ss << "(" << x << "," << y << ")";
    return ss.str();
  }
};

inline string to_string(const Point &p) {
  return p.to_string();
}

struct Angle {
  static Angle degrees(double d) {
    Angle a;
    a.set_degrees(d);
    return a;
  }

  static Angle radians(double rad) {
    Angle a;
    a.theta = rad;
    return a;
  }

  inline double radians() const {
    return theta;
  }
  inline double degrees() const  {
    return theta * 180. / M_PI;
  }
  inline void set_degrees(double d) {
    theta = d * M_PI/180.;
  }


  inline void standardize() {
   theta = fmod(theta + M_PI , 2.*M_PI) - M_PI;
  }

  const inline string to_string() {
    return std::to_string(degrees())+"°";
  }

  inline bool operator == (Angle& rhs)   {

    return theta == rhs.theta;
  }

  inline Angle & operator /= (double d)   {
    theta /= d;
    return *this;
  }


  inline Angle operator / (double d) const  {
    Angle rv;
    rv.theta = this->theta / d;
    return rv;
  }


  inline Angle operator * (double d) const  {
    Angle rv;
    rv.theta = this->theta * d;
    return rv;
  }

  inline Angle operator + (const Angle & rhs) const  {
    Angle rv;
    rv.theta = this->theta + rhs.theta;
    return rv;
  }

  inline Angle & operator += (const Angle & rhs) {
    theta += rhs.theta;
    return *this;
  }

  inline Angle operator -() {
    return Angle::radians(-theta);
  }

  inline Angle operator - (const Angle & rhs) const  {
    Angle rv;
    rv.theta = this->theta - rhs.theta;
    rv.standardize();
    return rv;
  }

private:
  //inline operator double & () {
  //  return theta;
 // }

  double theta; //radians
};

/*

from math import *
import numpy as np

#
# Angular geometry
#
*/
inline double degrees(double radians) {
  return radians * 180 / M_PI;
}

inline double radians(double degrees) {
  return degrees* M_PI / 180;
}


//#returns theta in range of [-pi,pi)
inline double standardized_radians(double theta) {
  return fmod(theta + M_PI , 2.*M_PI) - M_PI;
}
/*
#returns theta2-theta1 in range of [-pi,pi)
def radians_diff(theta1, theta2):
  return standardized_radians(theta2 - theta1)

*/
inline double standardized_degrees(double theta) {
  return  fmod(theta + 180., 360) - 180.;
}


// returns theta2-theta1 in range of [-180,180)
inline double degrees_diff(double theta1, double theta2) {
  return standardized_degrees(theta2 - theta1);
}
/*

def dot(x1,y1,x2,y2):
  return x1 * x2 + x2 * y2

def length(x,y):
  return sqrt(x**2+y**2)
*/
inline double length(double x, double y) {
  return sqrt(x*x+y*y);
}


//
// General geometry
//

inline vector<double> quadratic(double a,double b,double c) {

  return {(-b +sqrt(b*b - 4.*a*c))/(2.*a) ,(-b -sqrt(b*b-4.*a*c))/(2.*a)};
}

inline double distance(double x1, double y1, double x2, double y2) {
  return length(x2-x1,y2-y1);
}

inline double distance(Point p1, Point p2) {
  return distance(p1.x,p1.y,p2.x,p2.y);
}

//
// Kinematics
//

// returns first t greater than zero that will reach position x
inline double time_at_position(double x,double a,double v0,double x0=0.){
  x = x-x0;

  if(a==0)
    return x/v0;
  auto t = quadratic(0.5*a,v0,-x);
  if (t[0] < 0){
    return t[1] > 0 ? t[1] : NAN;
  }
  if (t[1] < 0){
    return t[0];
  }
  return min(t[0],t[1]);
}


inline double velocity_at_time(double t, double a, double v0){
  return v0 + a * t;
}

inline double velocity_at_position(double x, double a, double v0, double x0 = 0){
  x = x-x0;
  double t = time_at_position(x,a,v0);
  return velocity_at_time(t,a,v0);
}

inline Point unit_vector(Point p) {
  auto l=length(p.x,p.y);
  return Point(p.x/l,p.y/l);
}





// angle between two vectors
// based on http://stackoverflow.com/a/16544330/383967
inline Angle angle_between(double x1, double y1, double x2, double y2) {
  double dot = x1*x2 + y1*y2; // dot product
  double det = x1*y2 - y1*x2; // determinant
  return Angle::radians( atan2(det, dot) );
}

// returns direction from p1 to p2
inline Angle angle_to(Point p1, Point p2) {
  return Angle::radians(atan2(p2.y-p1.y,p2.x-p1.x));
}

// returns y for given x based on x1,y1,x2,y2
double interpolate(double x, double x1, double y1, double x2, double y2);



inline double clamp(double value, double min_value, double max_value) {
  if(value < min_value)
    return min_value;
  if (value > max_value)
    return max_value;
  return value;
}

inline vector<double> linspace(double from, double to, double step) {
  vector<double> v;
  for(double d = from; d <= to; d+= step) {
    v.push_back(d);
  }
  return v;
}


void test_geometry();



#endif // GEOMETRY_H
