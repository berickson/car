#include "geometry.h"
#include "iostream"


double interpolate(double x, double x1, double y1, double x2, double y2){
  double m = (y2 - y1)/( x2 - x1 );
  return y1 + m * (x-x1);
}

Point::Point(double x, double y) :
  x(x),y(y){}

string Point::to_string() const {
  stringstream ss;
  ss << "(" << x << "," << y << ")";
  return ss.str();
}

Point Point::operator -(Point rhs)
{
  return Point(x-rhs.x,y-rhs.y);
}

string to_string(const Point &p) {
  return p.to_string();
}

Angle Angle::degrees(double d) {
  Angle a;
  a.set_degrees(d);
  return a;
}

Angle Angle::radians(double rad) {
  Angle a;
  a.theta = rad;
  return a;
}

double Angle::radians() const {
  return theta;
}

double Angle::degrees() const  {
  return theta * 180. / M_PI;
}

void Angle::set_degrees(double d) {
  theta = d * M_PI/180.;
}

void Angle::set_radians(double theta_)
{
  theta = theta_;
}

void Angle::standardize() {
  theta = fmod(theta + M_PI , 2.*M_PI) - M_PI;
}

const string Angle::to_string() {
  return std::to_string(degrees())+"°";
}

bool Angle::operator ==(Angle &rhs)   {

  return theta == rhs.theta;
}

Angle &Angle::operator /=(double d)   {
  theta /= d;
  return *this;
}

Angle Angle::operator /(double d) const  {
  Angle rv;
  rv.theta = this->theta / d;
  return rv;
}

Angle Angle::operator *(double d) const  {
  Angle rv;
  rv.theta = this->theta * d;
  return rv;
}

Angle Angle::operator +(const Angle &rhs) const  {
  Angle rv;
  rv.theta = this->theta + rhs.theta;
  return rv;
}

Angle &Angle::operator +=(const Angle &rhs) {
  theta += rhs.theta;
  return *this;
}

Angle Angle::operator -() {
  return Angle::radians(-theta);
}

Angle Angle::operator -(const Angle &rhs) const  {
  Angle rv;
  rv.theta = this->theta - rhs.theta;
  rv.standardize();
  return rv;
}

double degrees(double radians) {
  return radians * 180 / M_PI;
}

double radians(double degrees) {
  return degrees* M_PI / 180;
}

double standardized_radians(double theta) {
  return fmod(theta + M_PI , 2.*M_PI) - M_PI;
}

double standardized_degrees(double theta) {
  return  fmod(theta + 180., 360) - 180.;
}

double degrees_diff(double theta1, double theta2) {
  return standardized_degrees(theta2 - theta1);
}

double length(double x, double y) {
  return sqrt(x*x+y*y);
}

vector<double> quadratic(double a, double b, double c) {

  return {(-b +sqrt(b*b - 4.*a*c))/(2.*a) ,(-b -sqrt(b*b-4.*a*c))/(2.*a)};
}

double distance(double x1, double y1, double x2, double y2) {
  return length(x2-x1,y2-y1);
}

double distance(Point p1, Point p2) {
  return distance(p1.x,p1.y,p2.x,p2.y);
}

// returns distance of point p from segment from start to end
double distance_from_segment_to_pointt(Point start, Point end, Point p ) {
  double dx = end.x - start.x;
  double dy = end.y - start.y;
  double drx = p.x - start.x;
  double dry = p.y - start.y;

  double progress = (drx * dx + dry * dy)/(dx * dx + dy * dy);
  if(progress < 0)
    return distance(start,p);
  if(progress > 1)
    return distance(end,p);
  double l = length(dx,dy);
  double cte = (dry * dx - drx * dy) / l;
  return fabs(cte);
}


double velocity_at_time(double t, double a, double v0){
  return v0 + a * t;
}

double velocity_at_position(double x, double a, double v0, double x0){
  x = x-x0;
  double t = time_at_position(x,a,v0);
  return velocity_at_time(t,a,v0);
}

Point unit_vector(Point p) {
  auto l=length(p.x,p.y);
  return Point(p.x/l,p.y/l);
}

Angle angle_between(double x1, double y1, double x2, double y2) {
  double dot = x1*x2 + y1*y2; // dot product
  double det = x1*y2 - y1*x2; // determinant
  return Angle::radians( atan2(det, dot) );
}

Angle angle_between(Point p1, Point p2) {
  return angle_between(p1.x,p1.y,p2.x,p2.y);
}

Angle angle_to(Point p1, Point p2) {
  return Angle::radians(atan2(p2.y-p1.y,p2.x-p1.x));
}


double clamp(double value, double min_value, double max_value) {
  if(value < min_value)
    return min_value;
  if (value > max_value)
    return max_value;
  return value;
}

vector<double> linspace(double from, double to, double step) {
  vector<double> v;
  for(double d = from; d <= to; d+= step) {
    v.push_back(d);
  }
  return v;
}

double acceleration_for_distance_and_velocities(double d, double v1, double v2) {
  bool same_signs = (v2 >= 0) == (v1 >= 0);

  double a;
  if(same_signs) {
    a = (v2*v2 - v1*v1) / (2*d);
  } else {
    a = (v2*v2 + v1*v1) / (2*d);
  }
  if(v2 < v1) {
    a = -a;
  }

  return a;
}

double time_at_position(double x, double a, double v0, double x0){
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


void test_acceleration_for_distance_and_velocities(double d, double v1, double v2) {
  cout << "from " << v1 << "m/s to " << v2 << "m/s in " << d
       <<  "m requires a of "
       <<  acceleration_for_distance_and_velocities(d, v1, v2)
       << "m/s^2" << endl;
}

void test_geometry() {
  test_acceleration_for_distance_and_velocities(1,0,3);
  test_acceleration_for_distance_and_velocities(2,0,3);
  test_acceleration_for_distance_and_velocities(1,-3,3);
  test_acceleration_for_distance_and_velocities(1,3,-3);
  test_acceleration_for_distance_and_velocities(1,-2,-3);
  Angle a;
  cout << "setting a to 30 degrees" << endl;
  a.set_degrees(30);
  cout << "a: " << a.to_string() << endl;
  cout << "a.degrees(): " << a.degrees() << endl;
  cout << "a.radians(): " << a.radians() << endl;
  cout << " a * 2" << (a * 2).to_string();
  a+=Angle::degrees(360);
  cout << "adding 360 degrees" << endl;
  cout << "a.degrees(): " << a.degrees() << endl;
  a.standardize();
  cout << "a.standardize() " << endl;
  cout << "a.degrees(): " << a.degrees() << endl;
  cout << "a /= 2" <<endl;
  a/=2.;
  cout << "a.degrees(): " << a.degrees() << endl;
  cout << "a.to_string(): " << a.to_string() << endl;

  a = Angle::degrees(10);
  Angle b = Angle::degrees(20);
  cout << "a + b " << (a+b).degrees() << endl;
}
