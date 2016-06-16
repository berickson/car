#ifndef ROUTE_H
#define ROUTE_H

#include <vector>
#include <string>

using namespace std;


struct RouteNode {
  double x;
  double y;
  double velocity;
  bool reverse;

  RouteNode(double _x=0.0, double _y=0.0, double _velocity = 0.0, bool _reverse=false) :
    x(_x),y(_y),velocity(_velocity),reverse(_reverse) {
  }
  void set_from_standard_file(vector<string> fields);
};

class Route {
public:
  Route(){}

  double distance();


  void load_from_file(string path);

  void optimize_velocity(double max_velocity = 1, double max_acceleration = 0.1);
  void add_node(RouteNode node);
  string to_string();
  string header_string();

  vector<string> columns = {"secs","x","y","rear_x", "rear_y", "reverse", "heading","adj","esc","str","m/s"};
  vector<RouteNode> nodes;
  int index = 0;
};

void test_route();

#endif // ROUTE_H
