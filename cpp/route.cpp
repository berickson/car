#include "route.h"
#include "geometry.h"
#include  <iostream>
#include <fstream>
#include <iomanip>
#include "string_utils.h"
#include <vector>
#include "logger.h"


// todo:  make separate object for current location

vector<string> Route::columns{"secs","x","y","rear_x", "rear_y", "reverse", "heading","adj","esc","str","m/s","road_sign_label","road_sign_command","arg1","arg2","arg3"};

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

string Route::to_string() {
  stringstream ss;
  ss<<Route::csv_header() << endl;
  for(auto node:nodes) {
    ss << node.csv_row() << endl;
  }
  return ss.str();
}

string Route::csv_header() {
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

Angle Route::get_heading_at_current_position()
{
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
  return start_angle + (end_angle - start_angle) * this->progress;
}

void Route::advance_to_next_segment()
{
  if (index >= nodes.size()-2) {
    done = true;
  }
  ++index;
}

bool Route::is_stop_ahead() {
  return done || nodes[index+1].road_sign_command == "stop";
}

RouteNode * Route::get_target_node()
{
  if (done) {
    return & nodes[index];
  }
  return & nodes[index+1];
}

void Route::set_position(Point front, Point rear, double velocity)
{
  const double stopped_velocity = 0.01;
  while(!done) {
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
      cte = (drx * dy - dry * dx ) / l;
    }

    if (progress < 1.0)
      break;

    // can advance nodes only if next node isn't a stop sign
    // to advance that, we need to mark as stopped
    if(p2.road_sign_command != "stop") {
      advance_to_next_segment();
    } else {
      break;
    }
  }
}


void Route::reset_position_to_start()
{
  index = 0;
  cte = 0;
  progress = 0;
  done = false;
}

double Route::get_acceleration() {
  if(done)
    return 0;

  const RouteNode & p0 = nodes[index];
  const RouteNode & p1 = nodes[index+1];
  double d = distance(p0.get_front_position(), p1.get_front_position());
  double a = acceleration_for_distance_and_velocities(d,p0.velocity,p1.velocity);
  return a;
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


bool RouteNode::has_road_sign() const {
  if(road_sign_label.length() > 0 || road_sign_command.length() > 0) {
    return true;
  }
  return false;
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
     << "heading_adjustment: " << 0.0 <<  " "
     << "esc: " << esc <<  " "
     << "str: " << str << " "
     << "reverse: " << reverse;
  return ss.str();
}


string RouteNode::csv_row(){
  stringstream ss;
  ss << secs << ","
     << front_x << ","
     << front_y << ","
     << rear_x << ","
     << rear_y << ","
     << reverse << ","
     << heading << ","
     << heading_adjustment << ","
     << esc << ","
     << str << ","
     << velocity << ","
     << road_sign_label << ","
     << road_sign_command << ","
     << arg1 << ","
     << arg2 << ","
     << arg3;

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
  if(fields.size() > 11) {
    if(fields.size() != 16) {
      log_error((string)"invalid field count: " + format(fields.size()) );
    }
    road_sign_label = fields[11];
    road_sign_command = fields[12];
    arg1 = fields[13];
    arg2 = fields[14];
    arg3 = fields[15];
  }
}

void Route::load_from_file(string path) {
  log_entry_exit w("Route::load_from_file");
  try {
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
    if(csv_header().compare(0, line.length(), line) != 0) {
      throw (string) "bad header for " + path + "was :" + line + " expected: " + csv_header();
    }
    nodes.clear();
    int line_number = 1;
    while(std::getline(f, line)) {
      ++line_number;
      trim(line);
      if(line=="") break;
      auto fields=split(line,',');
      const int min_field_count = 11;
      if(fields.size() < min_field_count) {
        stringstream ss;
        ss << "wrong number of columns in line " << line_number
            << " expected " << min_field_count
            << " was "  << fields.size();
        throw ss.str();
      }
      auto node = RouteNode();
      node.set_from_standard_file(fields);
      nodes.push_back(node);
      index = 0;
    }
  }
  catch(string s) {
    throw (string) "Error loading route: " + s ;
  }
  catch(...)
  {
    throw (string) "Unknown error loading route " + path;
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

void Route::prune(double max_segment_length, double tolerance)
{
  //return;
  // need at least 3 nodes to start pruning
  vector<int> nodes_to_prune;

  unsigned int i_segment_start = 0;
  unsigned int i_segment_end = 1;

  while(i_segment_end < nodes.size()) {
    const RouteNode & start_node = nodes[i_segment_start];
    const RouteNode & end_node = nodes[i_segment_end];
    bool segment_ok = true; // segment within tolerance
    Point s1 = start_node.get_front_position();
    Point s2 = end_node.get_front_position();
    if(distance(s1,s2)>max_segment_length || end_node.has_road_sign())
      segment_ok = false;
    for(unsigned int j=i_segment_start + 1; segment_ok && j < i_segment_end; j++) {
      // all candidates must be closer than tolerance
      const RouteNode & candidate= nodes[j];
      Point p = candidate.get_front_position();
      if(distance_from_segment_to_pointt(s1, s2, p) > tolerance){
        segment_ok = false;
      }
      if(end_node.has_road_sign()){
        segment_ok = false;
      }
    }
    if(segment_ok ) {
      ++i_segment_end;
    }
    else {
      for(unsigned int i_prune = i_segment_start + 1; i_prune < i_segment_end-1; ++i_prune) {
        nodes_to_prune.push_back(i_prune);
      }
      i_segment_start = i_segment_end;
      i_segment_end = i_segment_start + 1;
    }
  }

  log_info((string)"pruned "+format(nodes_to_prune.size()) + "nodes");
  for (auto it = nodes_to_prune.rbegin(); it != nodes_to_prune.rend(); ++it) {
   nodes.erase(nodes.begin()+*it);
  }
}

void Route::write_to_file(string path) {
  fstream f;
  f.open(path, ios_base::out);
  f<<Route::csv_header() << endl;
  for(auto node:nodes) {
    f << node.csv_row() << endl;
  }
}

void Route::optimize_velocity(double max_velocity, double max_lateral_acceleration, double max_acceleration, double max_deceleration) {
  if(is_nan(max_acceleration)) {
    max_acceleration = max_lateral_acceleration;
  }
  if(is_nan(max_deceleration)) {
    max_deceleration = max_acceleration;
  }
  // must have at least one node to optimize
  if(nodes.size() == 0)
    return;

  // all velocities start at max
  for(RouteNode &node:nodes) {
    node.velocity = max_velocity;
  }

  // final velocity must be zero
  nodes[nodes.size()-1].velocity = 0.0;

  // all "stop" node velocities must be zero
  for(RouteNode & node : nodes) {
    if(node.road_sign_command == "stop") {
      node.velocity = 0.0;
    }
  }

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

    Angle theta = angle_between(v1,v2);
    if(fabs(theta.radians())>0) {
      double r=fabs(length(v2.x,v2.y)/theta.radians());
      double v_circle_max = sqrt(max_lateral_acceleration * r);
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
    p0.velocity = min(p0.velocity, velocity_at_position(ds,max_deceleration,p1.velocity));
  }

  // starting velocity must be zero
  // apply speed up / acceleration
  // from start to end
  nodes[0].velocity = 0;
  for(int i=0; i < (int)nodes.size()-2; i++) {
    RouteNode & p0 = nodes[i];
    RouteNode & p1 = nodes[i+1];
    double ds = ::distance(p0.front_x,p0.front_y,p1.front_x,p1.front_y);
    p1.velocity = min(p1.velocity, velocity_at_position(ds,max_acceleration,p0.velocity));
  }
}
