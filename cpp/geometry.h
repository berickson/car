#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "math.h"
#include <string>
#include <vector>
#include <sstream>


using namespace std;

struct Point {
  Point(double x=0, double y=0);
  double x;
  double y;
  string to_string() const;
  Point operator -(Point p2);
};

string to_string(const Point &p);

struct Angle {
  static Angle degrees(double d);

  static Angle radians(double rad);

  double radians() const;
  double degrees() const;
  void set_degrees(double d);
  void set_radians(double theta);

  void standardize();

  const string to_string();

  bool operator == (Angle& rhs);
  Angle & operator /= (double d);
  Angle operator / (double d) const;
  Angle operator * (double d) const;
  Angle operator + (const Angle & rhs) const;
  Angle & operator += (const Angle & rhs);
  Angle operator -();
  Angle operator - (const Angle & rhs) const;

private:

  double theta; //radians
};


//
// Angular geometry
//

double degrees(double radians);
double radians(double degrees);


//#returns theta in range of [-pi,pi)
double standardized_radians(double theta);
double standardized_degrees(double theta);


// returns theta2-theta1 in range of [-180,180)
double degrees_diff(double theta1, double theta2);

// returns length of vector (x,y)
double length(double x, double y);

//
// General geometry
//

// returns solutions to ax^2+bx+c=0
vector<double> quadratic(double a,double b,double c);

// returns distane from (x1,y1) to (x2,y2)
double distance(double x1, double y1, double x2, double y2);
double distance(Point p1, Point p2);

//
// Kinematics
//

// returns first t greater than zero that will reach position x
double time_at_position(double x,double a,double v0,double x0=0.);

// returns velocity at time t with initial velocity v0 and acceleration a
double velocity_at_time(double t, double a, double v0);

// returns velocity at position x with acceleration a, initial velocity v0 and initial position x0
double velocity_at_position(double x, double a, double v0, double x0 = 0);

// returns unit length vector in the direction of p
Point unit_vector(Point p);

// angle between two vectors
// based on http://stackoverflow.com/a/16544330/383967
Angle angle_between(double x1, double y1, double x2, double y2);
Angle angle_between(Point p1, Point p2);

// returns direction from p1 to p2
Angle angle_to(Point p1, Point p2);

// returns y for given x based on x1,y1,x2,y2
double interpolate(double x, double x1, double y1, double x2, double y2);

double clamp(double value, double min_value, double max_value);

vector<double> linspace(double from, double to, double step);


void test_geometry();



#endif // GEOMETRY_H
