#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "../route.h"

using namespace std;
using namespace testing;

class RouteBuilder {
public:
  RouteBuilder(Route & route, double car_length = 1.0) :
    route(route){
    this->car_length = car_length;
  }
  Route & route;
  RouteNode & get_head() {
    return route.nodes.at(route.nodes.size()-1);
  }

  double car_length = NAN;
  void add_node(double x, double y){
    if(route.nodes.size() == 0) {
      RouteNode zero;
      zero.secs = 0;
      zero.front_x = 0;
      zero.front_y = 0;
      zero.heading = 0;
      zero.heading_adjustment = 0;
      zero.rear_x = -car_length;
      zero.rear_y = -car_length;
      route.add_node(zero);
    }
    RouteNode n;
    n.front_x = x;
    n.front_y = y;
    RouteNode & head = get_head();

    Angle angle =  angle_to(head.get_front_position(), n.get_front_position());
    n.heading = angle.degrees();
    head.heading = angle.degrees();
    n.heading_adjustment = 0;
    n.secs = route.nodes.size();


    n.rear_x = n.front_x - car_length * cos(angle.radians());
    n.rear_y = n.front_y - car_length * sin(angle.radians());

    head.rear_x = head.front_x - car_length * cos(angle.radians());
    head.rear_y = head.front_y - car_length * sin(angle.radians());
    route.add_node(n);

  }
};


Route get_circle(double r = 1, unsigned steps = 360*10) {
  Route route;
  RouteBuilder builder(route);

  for(unsigned i = 1; i < steps; i++) {
    Angle theta = Angle::radians(2*M_PI * i / (steps-1));
    double x = r * sin(theta.radians());
    double y = r - r * cos(theta.radians());
    builder.add_node(x,y);
  }
  return route;
}


void test_circle_acceleration(double radius, double max_a, double prune_tolerance = 0.1) {
  Route route = get_circle(radius);
  EXPECT_NEAR(route.get_length(), 2 * M_PI * radius, 0.01);

  cout << "circle with radius " << radius  << endl;
  cout << "route distance for circle: " << route.get_length() << endl;
  cout << "optimized_velocity with max_a = " << max_a << endl;

  route.optimize_velocity(99999,max_a);
  float v = route.get_max_velocity();
  EXPECT_NEAR(v*v/radius,max_a,0.01);
  cout << "max_v = " << route.get_max_velocity() << endl;
}

TEST(route,  test_circle_acceleration) {
  // note: using larger circles so
  // ramp-up and down times don't affect
  // maximum velocity
  test_circle_acceleration(20,1);
  test_circle_acceleration(40,1);
}

TEST(route, test_prune) {
  Route r;
  r.add_node(RouteNode(0,0));
  r.add_node(RouteNode(1,0));
  r.add_node(RouteNode(2,.1));
  r.add_node(RouteNode(3,0));
  r.add_node(RouteNode(4,0));
  r.add_node(RouteNode(5,0));
  r.add_node(RouteNode(6,0));
  r.add_node(RouteNode(7,0));
  r.add_node(RouteNode(8,0));
  cout << r.to_string() << endl;
  r.prune(3,0.1);
  cout << r.to_string() << endl;

}

TEST(route, get_curvature_at_current_position) {
  Route r;
  r.add_node(RouteNode(0,0));
  r.add_node(RouteNode(1,0));
  r.add_node(RouteNode(2,.1));
  r.add_node(RouteNode(3,0));
  r.add_node(RouteNode(4,0));
  r.add_node(RouteNode(5,0));
  r.add_node(RouteNode(6,0));
  for(double d = 0; d < 8.; d+=0.1) {
    r.set_position(Point(d,0),Point(d-1,0),1.0);
    cout << "d: " << d
         << " c: "<< r.get_curvature_at_current_position().degrees()
         << "heading:  " << r.get_heading_at_current_position().degrees()
         << endl;

  }
}

TEST(route, straight_line_velocity) {
  Route r;
  r.add_node(RouteNode(0,0));
  r.add_node(RouteNode(1,0));
  r.add_node(RouteNode(2,0));
  r.add_node(RouteNode(3,0));
  r.add_node(RouteNode(4,0));
  r.add_node(RouteNode(5,0));
  r.add_node(RouteNode(6,0));
  r.add_node(RouteNode(7,0));
  r.add_node(RouteNode(8,0));
  r.add_node(RouteNode(9,0));
  r.optimize_velocity(1000, 0.1, 0.01, 0.01);
  cout << "front_x,front_y,velocity" << endl;
  for(auto n : r.nodes) {
    cout << n.front_x << ", " << n.front_y << ", " <<  n.velocity << endl;
  }

}

TEST(route, optimize_velocity) {
  Route r;
  r.add_node(RouteNode(0,0));
  r.add_node(RouteNode(1,0));
  r.add_node(RouteNode(2,0));
  r.add_node(RouteNode(3,0));
  r.add_node(RouteNode(4,0));
  r.add_node(RouteNode(5,0));
  r.add_node(RouteNode(6,0));
  r.add_node(RouteNode(7,0));
  r.add_node(RouteNode(7,0));
  r.add_node(RouteNode(9,0));
  r.add_node(RouteNode(0,0));
  r.optimize_velocity();
  cout << r.to_string();
  cout << "max velocity: " << r.get_max_velocity() << endl;;
}


TEST(route, usages) {
  //test_optimize_velocity();
  return;
  //test_prune();
  //return;
  //test_curvature();
  //return;
  // test_circle();
  double max_a = 8.;
  double max_v = 10000;
  Route r;
  r.load_from_file("/home/brian/car/tracks/2016-09-12 prisk/routes/F/path.csv");
  cout << "original distance: " << r.get_length() << endl;
  cout << "original curvature: " << r.get_total_curvature().degrees() << " degrees" << endl;
  cout << "original max velocity" << r.get_max_velocity() << endl;
  r.optimize_velocity(max_v, max_a);
  cout << "optimized max velocity " << r.get_max_velocity() << endl;
  r.smooth(0.25);
  r.optimize_velocity(max_v, max_a);

  cout << "smoothing " << endl;
  cout << "distance after smooth: " << r.get_length() << endl;
  cout << "curvature after smooth: " << r.get_total_curvature().degrees() << " degrees" << endl;
  cout << "point ahead 0.2 " << r.get_position_ahead(0.2).to_string() << endl;
  cout << "point ahead 100 " << r.get_position_ahead(100).to_string() << endl;
  r.optimize_velocity(max_v, max_a);
  //cout << r.to_string() << endl;
  cout << "max velocity after smooth " << r.get_max_velocity() << endl;
  cout << "node count " << r.nodes.size() << endl;
  r.prune(5.0, 0.02);
  r.optimize_velocity(max_v, max_a);
  cout << "max velocity after prune " << r.get_max_velocity() << endl;
  cout << "node count after prune " << r.nodes.size() << endl;
  cout << "curvature after prune: " << r.get_total_curvature().degrees() << " degrees" << endl;
  cout << "point ahead 0.2 " << r.get_position_ahead(0.2).to_string() << endl;
  cout << "point ahead 100 " << r.get_position_ahead(100).to_string() << endl;
  //cout << r.to_string() << endl;

}
