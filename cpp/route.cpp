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

// returns heading based on current path segment
Angle Route::heading()
{
  RouteNode & p1 = nodes[index];
  RouteNode & p2 = nodes[index+1];
  Angle a = Angle::radians(atan2(p2.y-p1.y,p2.x-p1.x));
  if(p1.reverse)
    a += Angle::radians(M_PI);
  a.standardize();
  return a;
}

void Route::set_position(Point &front, Point &rear, double velocity)
{
  const double stopped_velocity = 0.01;
  while(true) {
    RouteNode & p1 = nodes[index];
    RouteNode & p2 = nodes[index+1];
    double dx = NAN;
    double dy = NAN;
    double drx = NAN;
    double dry = NAN;

    if(abs(velocity) < stopped_velocity && abs(p2.velocity) < stopped_velocity) {
      progress = 1.0; // will move to next node
    } else {
      if(p1.reverse) {
        // calculate rear position if not in route
        if(isnan(p1.rear_x)) {
          double h = heading().radians();
          double car_l = ::distance(front,rear);
          double rear_offset_x = -car_l* cos(h);
          double rear_offset_y = -car_l* sin(h);
          dx = p2.x-p1.x;
          dy = p2.y-p1.y;
          drx = rear.x - (p1.x + rear_offset_x);
          dry = rear.y - (p1.y + rear_offset_y);

        } else {
          // d from p1 to p2
          dx = p2.rear_x-p1.rear_x;
          dy = p2.rear_y-p1.rear_y;

          // d from p1 to robot
          drx = rear.x - p1.rear_x;
          dry = rear.y - p1.rear_y;
        }
      } else {

        // d from p1 to p2
        dx = p2.x-p1.x;
        dy = p2.y-p1.y;

        // d from p1 to robot
        drx = rear.x - p1.x;
        dry = rear.y - p1.y;
      }
    }

    double l = length(dx,dy);
    if(l < 0.000001) {
      progress = 1.1;
    } else {
      progress = (drx * dx + dry + dy)/(dx * dx + dy * dy);
      cte = (dry * dx - drx * dy) / l;
    }

    if (progress < 1.0 || done)
      break;
    if (index >= nodes.size()-2) {
      done = true;
      break;
    }
    ++index;

  }

}

// returns desired velocity for current position
double Route::get_velocity()
{
  if(done)
    return 0;
  auto p0 = nodes[index];
  auto p1 = nodes[index+1];
  double v = NAN;

  // if we just turned around, take the velocity from the node in the new direction
  if(index > 0 && nodes[index-1].reverse != p1.reverse){
    v = p1.velocity;
  } else if (progress < 0) {
    v =  p0.velocity;
  } else if (progress > 1){
    v = p1.velocity;
  } else {
    v = p0.velocity + (p1.velocity - p0.velocity) * progress;
  }

  // set negative speed for reverse
  if (p0.reverse){
    v = -v;
  }
  return v ;
}

Point Route::get_position_ahead(double d)
{
  unsigned i = index;
  double segment_progress = progress;
  while(true) {
    RouteNode & p1 = nodes[i];
    RouteNode & p2 = nodes[i+1];
    double dx = p2.x-p1.x;
    double dy = p2.y-p1.y;
    double l = ::length(dx,dy);
    double progress_d = l * segment_progress;
    double remaining_d = l - progress_d;

    // go to next node
    // if we are past the end of this node
    // and there are nodes left
    if (d > remaining_d && i < nodes.size()-2) {
      ++i;
      d -= remaining_d;
      segment_progress = 0;
      continue;
    }
    double unit_x = dx/l;
    double unit_y = dy/l;
    double x = p1.x + segment_progress * dx;
    double y = p1.y + segment_progress * dy;
    double ahead_x = x + unit_x * d;
    double ahead_y = y + unit_y * d;
    return Point(ahead_x, ahead_y);
  }
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
