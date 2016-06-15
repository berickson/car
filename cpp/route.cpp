#include "route.h"
#include "geometry.h"
#include  <iostream>
#include <iomanip>

void Route::add_node(Route::Node node){
  nodes.push_back(node);
}

double Route::distance() {
  double d = 0;
  for(unsigned i=0;i<nodes.size()-1;i++){
    auto n1=nodes[i];
    auto n2=nodes[i+1];
    d+=::distance(n1.x,n1.y,n2.x,n2.y);
  }
  return d;
}

Route get_circle(double r = 1, unsigned steps = 300) {
  Route route;

  for(unsigned i = 0; i < steps; i++) {
    Angle theta = Angle::radians(2*M_PI * i / (steps-1));
    double x = r * sin(theta.radians());
    double y = r - r * cos(theta.radians());
    route.add_node(Route::Node(x,y));
  }
  return route;
}

string Route::to_string() {
  stringstream ss;
  for(auto node:nodes) {
    ss << setprecision(3) << "x:"  << node.x << " y:" << node.y << " v: " << node.velocity << " reverse:" << node.reverse << endl;
  }
  return ss.str();
}


void Route::optimize_velocity(double max_velocity, double max_acceleration) {

  // all velocities start at max
  for(Route::Node &node:nodes) {
    node.velocity = max_velocity;
  }

  // end velocity must be zero
  nodes[nodes.size()-1] = 0;

  // final velocity must be zero
  nodes[nodes.size()-1].velocity = 0.0;

  // apply speed limit of zero for switching direction
  for(unsigned i=0;i<nodes.size()-2; i++) {
    Node & p0 = nodes[i];
    Node & p1 = nodes[i+1];
    if(p0.reverse != p1.reverse) {
      p1.velocity = 0.0;
    }
  }

  // apply speed limits for curves
  for(unsigned i=0; i < nodes.size()-3; i++) {
    Node & p0 = nodes[i];
    Node & p1 = nodes[i+1];
    Node & p2 = nodes[i+2];

    // find two vectors for this and next segment
    Point v1(p1.x-p0.x, p1.y-p0.y);
    Point v2(p2.x-p1.x, p2.y-p1.y);

    Angle theta = angle_between(v1.x,v1.y,v2.x,v2.y);
    if(fabs(theta.radians())>0) {
      double r=fabs(length(v2.x,v2.y)/theta.radians());
      double v_circle_max = sqrt(max_acceleration * r);
      p1.velocity = min(p1.velocity, v_circle_max);
    }
  }

  // todo: consider lateral acceleration along with deceleration

  // apply slow down / stopping acceleration
  // from end to start
  for(int i = nodes.size()-2; i >= 0; --i) {
    Node & p0 = nodes[i];
    Node & p1 = nodes[i+1];
    double ds = ::distance(p0.x,p0.y,p1.x,p1.y);
    p0.velocity = min(p0.velocity, velocity_at_position(ds,max_acceleration,p1.velocity));
  }
}



void test_circle() {
  Route r = get_circle(2);
  cout << "route distance for circle: " << r.distance() << endl;
  cout << r.to_string();
  r.optimize_velocity();
  cout << "optimized_velocity defaults" << endl;
  cout << r.to_string();

  r = get_circle(10);
  cout << "route distance for circle: " << r.distance() << endl;
  cout << r.to_string();
  r.optimize_velocity();
  cout << "optimized_velocity defaults" << endl;
  cout << r.to_string();

}

void test_route() {
  test_circle();
}
