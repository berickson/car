#include "route.h"
#include "geometry.h"
#include  <iostream>
#include <fstream>
#include <iomanip>
#include "string_utils.h"

void Route::add_node(RouteNode node){
  nodes.push_back(node);
}

double Route::get_length() {
  double d = 0;
  for(unsigned i=0;i<nodes.size()-1;i++){
    auto n1=nodes[i];
    auto n2=nodes[i+1];
    d+=::distance(n1.front_x,n1.front_y,n2.front_x,n2.front_y);
  }
  return d;
}

Angle Route::get_total_curvature()
{
  double radians = 0;
  for(unsigned i=0;i<nodes.size()-2;i++){
    auto n1=nodes[i];
    auto n2=nodes[i+1];
    auto n3=nodes[i+2];
    Point v1(n2.front_x-n1.front_x, n2.front_y-n1.front_y);
    Point v2(n3.front_x-n2.front_x, n3.front_y-n2.front_y);
    radians += fabs(::angle_between(v1,v2).radians());

  }
  return Angle::radians(radians);

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
    ss << node.to_string() << endl;
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
  Angle a = Angle::radians(atan2(p2.front_y-p1.front_y,p2.front_x-p1.front_x));
  if(p1.reverse)
    a += Angle::radians(M_PI);
  a.standardize();
  return a;
}

bool is_nan(double __x)
  { return __builtin_isnan(__x); }

// returns curvature in radians per meter
Angle Route::get_curvature_at_current_position() {

  Angle start_angle;

  Point p1 = nodes[index].get_front_position();
  Point p2 = nodes[index+1].get_front_position();
  // get the start angle
  if(index == 0) {
    start_angle.set_radians(0);
  } else {
    Point p0 = nodes[index-1].get_front_position();
    start_angle = angle_to(p0, p1);
  }
  Angle end_angle = angle_to(p1,p2);

  return (end_angle - start_angle) / distance(p2,p1);



}

void Route::set_position(Point front, Point rear, double velocity)
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
          // d from p1 to p2
          dx = p2.rear_x-p1.rear_x;
          dy = p2.rear_y-p1.rear_y;

          // d from p1 to robot
          drx = rear.x - p1.rear_x;
          dry = rear.y - p1.rear_y;
      } else {

        // d from p1 to p2
        dx = p2.front_x-p1.front_x;
        dy = p2.front_y-p1.front_y;

        // d from p1 to robot
        drx = front.x - p1.front_x;
        dry = front.y - p1.front_y;
      }
    }

    double l = length(dx,dy);
    if(l < 0.000001) {
      progress = 1.1;
    } else {
      progress = (drx * dx + dry * dy)/(dx * dx + dy * dy);
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
    v = -fabs(v);
  }
  return v ;
}

RouteNode Route::get_position_ahead(double d)
{
  unsigned i = index;
  double segment_progress = progress;
  while(true) {
    RouteNode p1 = nodes[i];
    RouteNode p2 = nodes[i+1];
    double dx = p2.front_x-p1.front_x;
    double dy = p2.front_y-p1.front_y;
    double l = ::length(dx,dy);
    double progress_d = l * segment_progress;
    double remaining_d = l - progress_d;

    // go to next node
    // if we are past the end of this node
    // and there are nodes left
    if (d > remaining_d && i < nodes.size()-2 ) {
      if(p1.reverse != p2.reverse)
        return p2;
      ++i;
      d -= remaining_d;
      segment_progress = 0;
      continue;
    }

    double fraction = d / l;
    // clamp fraction to 1, handles end point case where we go off the end of the route
    fraction = min(fraction, 1.);

    // interpolate node between p1 and p2
    RouteNode rv;
    rv.front_x = p1.front_x + fraction * (p2.front_x-p1.front_x);
    rv.front_y = p1.front_y + fraction * (p2.front_y-p1.front_y);
    rv.rear_x = p1.rear_x + fraction * (p2.rear_x-p1.rear_x);
    rv.rear_y = p1.rear_y + fraction * (p2.rear_y-p1.rear_y);
    rv.secs = p1.secs + fraction * (p2.secs-p1.secs);
    rv.heading = p1.heading + fraction * (p2.heading-p1.heading);
    rv.velocity = p1.velocity+ fraction * (p2.velocity-p1.velocity);
    rv.str = p1.str+ fraction * (p2.str-p1.str);
    rv.esc = p1.esc+ fraction * (p2.esc-p1.esc);
    rv.reverse = p1.reverse;

    return rv;
  }
}


string RouteNode::to_string()
{
  stringstream ss;
  ss << "secs: " << secs << " "
     << "x: " << front_x <<  " "
     << "y: " << front_y <<  " "
     << "rear_x: " << rear_x <<  " "
     << "rear_y: " << rear_y <<  " "
     << "heading: " << heading <<  " "
     << "velocity: " << velocity <<  " "
     << "heading_adjustment: " << heading_adjustment <<  " "
     << "esc: " << esc <<  " "
     << "str: " << str << " "
     << "reverse: " << reverse;
  return ss.str();
}

void RouteNode::set_from_standard_file(vector<string> fields) {
  secs = stod(fields[0]);
  front_x = stod(fields[1]);
  front_y = stod(fields[2]);
  rear_x = stod(fields[3]);
  rear_y = stod(fields[4]);
  reverse = stoi(fields[5])>0;
  heading = stod(fields[6]);
  heading_adjustment = stod(fields[7]);
  esc = stod(fields[8]);
  str = stod(fields[9]);
  velocity = stod(fields[10]);
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


vector<Point> smooth_points(vector<Point> & path, double weight_smooth = 0.5, double tolerance = 0.01) {
  if(weight_smooth >=1 ) throw (string) "weight_smooth must be less than one";
  if(weight_smooth <=0 ) throw (string) "weight_smooth must be greater than zero";
  if(tolerance <=0 ) throw (string) "tolerance must be greater than zero";

  double weight_data = 1. - weight_smooth;


  vector<Point> new_path(path); // make copy of input
  double change = tolerance;
  int iter = 0;
  while(change >= tolerance && iter < 100) {
    ++iter;
    change = 0;
    // loop through all middle nodes
    for(int i = 1; i < (int)path.size()-1; ++i) {
      Point aux = new_path[i];

      new_path[i].x += weight_data * (path[i].x - new_path[i].x);
      new_path[i].x += weight_smooth * (path[i-1].x + new_path[i+1].x - 2.0 * new_path[i].x);
      change += abs(aux.x-new_path[i].x);

      new_path[i].y += weight_data * (path[i].y - new_path[i].y);
      new_path[i].y += weight_smooth * (path[i-1].y + new_path[i+1].y - 2.0 * new_path[i].y);
      change += abs(aux.y-new_path[i].y);
    }
  }
  return new_path;

}

void Route::smooth(double k_smooth) {
  // todo: do the sommothing piece-wise for forward and revers
  vector<Point> path;
  for(const RouteNode & node:nodes) {
    path.push_back(node.get_front_position());
  }
  vector<Point> smoothed = smooth_points(path, k_smooth);

  for(unsigned i = 0; i < smoothed.size(); i++) {
    const Point & s = smoothed[i];
    RouteNode & n = nodes[i];
    double dx = s.x - n.front_x;
    double dy = s.y - n.front_y;
    n.front_x += dx;
    n.rear_x += dx;
    n.front_y += dy;
    n.rear_y += dy;
  }

}


void Route::optimize_velocity(double max_velocity, double max_acceleration) {

  // all velocities start at max
  for(RouteNode &node:nodes) {
    node.velocity = max_velocity;
  }

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
  for(int i=0; i < (int)nodes.size()-3; i++) {
    RouteNode & p0 = nodes[i];
    RouteNode & p1 = nodes[i+1];
    RouteNode & p2 = nodes[i+2];

    // find two vectors for this and next segment
    Point v1(p1.front_x-p0.front_x, p1.front_y-p0.front_y);
    Point v2(p2.front_x-p1.front_x, p2.front_y-p1.front_y);

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
    double ds = ::distance(p0.front_x,p0.front_y,p1.front_x,p1.front_y);
    p0.velocity = min(p0.velocity, velocity_at_position(ds,max_acceleration,p1.velocity));
  }
}



void test_circle() {
  Route r = get_circle(2);
  cout << "route distance for circle: " << r.get_length() << endl;
  cout << r.to_string();
  r.optimize_velocity();
  cout << "optimized_velocity defaults" << endl;
  cout << r.to_string();

  r = get_circle(10);
  cout << "route distance for circle: " << r.get_length() << endl;
  cout << r.to_string();
  r.optimize_velocity();
  cout << "optimized_velocity defaults" << endl;
  cout << r.to_string();

}

void test_curvature() {
  Route r;
  r.add_node(RouteNode(0,0));
  r.add_node(RouteNode(1,0));
  r.add_node(RouteNode(2,.5));
  r.add_node(RouteNode(3,0));
  r.add_node(RouteNode(4,0));
  r.add_node(RouteNode(5,0));
  r.add_node(RouteNode(6,0));
  for(double d = 0; d < 8.; d+=0.1) {
    r.set_position(Point(d,0),Point(d-1,0),1.0);
    cout << "d: " << d << " c: "<< r.get_curvature_at_current_position().degrees() << endl;
  }
}

void test_route() {
  test_curvature();
  return;
  // test_circle();
  Route r;
  r.load_from_file("/home/brian/car/tracks/sidewalk/routes/K/path.csv");
  cout << "distance before smooth: " << r.get_length() << endl;
  cout << "curvature before smooth: " << r.get_total_curvature().degrees() << " degrees" << endl;
  r.smooth(0.25);
  cout << "smoothing " << endl;
  cout << "distance after smooth: " << r.get_length() << endl;
  cout << "curvature after smooth: " << r.get_total_curvature().degrees() << " degrees" << endl;

  cout << "point ahead 0.2 " << r.get_position_ahead(0.2).to_string() << endl;
  cout << "point ahead 100 " << r.get_position_ahead(100).to_string() << endl;

}
