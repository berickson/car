#include "route.h"
#include "geometry.h"
#include  <iostream>
#include <fstream>
#include <iomanip>
#include "string_utils.h"

void Route::add_node(RouteNode node){
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
    route.add_node(RouteNode(x,y));
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

string Route::header_string() {
  return join(columns,",");
}


void RouteNode::set_from_standard_file(vector<string> fields) {
  x = stod(fields[1]);
  y = stod(fields[2]);
  velocity = stod(fields[10]);
  reverse = fields[5]=="True";
}

void Route::load_from_file(string path) {
  fstream f;
  f.open(path,ios_base::in);
  if(!f.is_open()) {
    throw (string) "could not open route at " + path;
  }

  string line;
  // read header
  if(!std::getline(f,line)) {
    throw (string) "error reading header for " + path;
  }
  trim(line);
  if(line != header_string()) {
    throw (string) "bad header for " + path;
  }
  nodes.clear();
  int line_number = 1;
  while(std::getline(f, line)) {
    ++line_number;
    trim(line);
    if(line=="") break;
    auto fields=split(line,',');
    if(fields.size()!=columns.size()) {
      stringstream ss;
      ss << "wrong number of columns in line " << line_number
          << " expected " << columns.size()
          << " was "  << fields.size();
      throw ss.str();
    }
    auto node = RouteNode();
    node.set_from_standard_file(fields);
    nodes.push_back(node);
    index = 0;
  }
}


double Route::get_max_velocity() {
  double max_v = 0.0;
  for(RouteNode& node:nodes) {
    max_v = max(max_v,node.velocity);
  }
  return max_v;
}


void Route::smooth(double k_smooth) {

}


void Route::optimize_velocity(double max_velocity, double max_acceleration) {

  // all velocities start at max
  for(RouteNode &node:nodes) {
    node.velocity = max_velocity;
  }

  // end velocity must be zero
  nodes[nodes.size()-1] = 0;

  // final velocity must be zero
  nodes[nodes.size()-1].velocity = 0.0;

  // apply speed limit of zero for switching direction
  for(unsigned i=0;i<nodes.size()-2; i++) {
    RouteNode & p0 = nodes[i];
    RouteNode & p1 = nodes[i+1];
    if(p0.reverse != p1.reverse) {
      p1.velocity = 0.0;
    }
  }

  // apply speed limits for curves
  for(unsigned i=0; i < nodes.size()-3; i++) {
    RouteNode & p0 = nodes[i];
    RouteNode & p1 = nodes[i+1];
    RouteNode & p2 = nodes[i+2];

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
    RouteNode & p0 = nodes[i];
    RouteNode & p1 = nodes[i+1];
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
  // test_circle();
  Route r;
  r.load_from_file("/home/brian/car/tracks/back yard/routes/A/path.csv");
  cout << r.to_string();

}
