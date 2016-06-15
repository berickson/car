#ifndef ROUTE_H
#define ROUTE_H

#include <vector>
#include <string>

using namespace std;

class Route {
public:
  Route(){}

  double distance();

  struct Node {
    double x;
    double y;
    double velocity;
    bool reverse;

    Node(double _x=0.0, double _y=0.0, double _velocity = 0.0, bool _reverse=false) :
      x(_x),y(_y),velocity(_velocity),reverse(_reverse) {

    }
  };

  void optimize_velocity(double max_velocity = 1, double max_acceleration = 0.1);
  void add_node(Node node);
  string to_string();

  vector<string> columns;// = {"secs","x","y","rear_x", "rear_y", "reverse", "heading","adj","esc","str","m/s"};
  vector<Node> nodes;
};

void test_route();

#endif // ROUTE_H
