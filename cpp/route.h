#ifndef ROUTE_H
#define ROUTE_H

#include <vector>
#include <string>
#include "geometry.h"
#include "math.h"

using namespace std;


struct RouteNode {
  double secs=NAN;
  double rear_x=NAN;
  double rear_y=NAN;
  double heading=NAN;
  double heading_adjustment=NAN;
  double esc=NAN;
  double str=NAN;

  double front_x;
  double front_y;
  double velocity;
  bool reverse;

  string road_sign_label;
  string road_sign_command;
  string arg1;
  string arg2;
  string arg3;

  bool has_road_sign() const;

  Point get_front_position() const {
    return Point(front_x,front_y);
  }

  string to_string();

  RouteNode(double _x=0.0, double _y=0.0, double _velocity = 0.0, bool _reverse=false) :
    front_x(_x),front_y(_y),velocity(_velocity),reverse(_reverse) {
  }
  void set_from_standard_file(vector<string> fields);

public:
  string csv_row();
};

class Route {
public:
  Route(){}

  double get_length();
  Angle get_total_curvature();


  void load_from_file(string path);

  void smooth(double k_smooth);
  void prune(double max_segment_length, double tolerance);

  void optimize_velocity(double max_velocity = 1, double max_lateral_acceleration = 0.1, double max_acceleration = NAN, double max_deceleration = NAN);
  double get_max_velocity();

  void add_node(RouteNode node);
  string to_string();
  static string csv_header();

  Angle heading();

  void set_position(Point front, Point rear, double velocity);
  void reset_position_to_start();

  double get_velocity();
  RouteNode get_position_ahead (double get_length) const;

  static vector<string> columns;
  vector<RouteNode> nodes;
  unsigned int index = 0;
  double cte = 0;
  double progress = 0;
  bool done = false;
  Angle get_curvature_at_current_position();
  Angle get_heading_at_current_position();
  double get_acceleration();
  void write_to_file(string path);
  void advance_to_next_segment();
  bool is_stop_ahead();
  RouteNode * get_source_node();
  RouteNode * get_target_node();
};

void test_route();

#endif // ROUTE_H
