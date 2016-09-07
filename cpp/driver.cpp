#include "driver.h"
#include "car.h"
#include "math.h"
#include "car_ui.h"
#include <unistd.h> // usleep


Driver::Driver(Car & car, CarUI ui, RunSettings settings)
  : car(car), ui(ui)
{
  t_ahead = settings.t_ahead;
  d_ahead = settings.d_ahead;
  k_smooth = settings.k_smooth;
}

// todo: replace all of this with a proper PID style loop
int esc_for_velocity(double goal_velocity, Car & car) {

  // special case for goal velocity of zero, always use neutral
  if (goal_velocity == 0) {
    return car.esc_for_velocity(0);
  }

  double esc_ms = NAN;

  // calculate speed
  double car_velocity = car.get_velocity();
  double error = car_velocity - goal_velocity;

  if (goal_velocity < 0){
    // reverse
    const double speed_up_esc = car.min_reverse_esc - 80;
    const double slow_down_esc = 1500;
    const double maintain_esc = car.min_reverse_esc;

    if (error > 0) {
        esc_ms = speed_up_esc;
    } else if (error < 0.2){
      esc_ms = slow_down_esc;
    } else {
        esc_ms = maintain_esc;
    }
  } else {
    // forward
    if (error > 0.2) {
      esc_ms = 1500; // car.min_reverse_esc # slow down esc
    } else if (error > 0.) {
      esc_ms = car.esc_for_velocity(clamp(goal_velocity- 3.*error,0.1,999.));
    } else {
      esc_ms = car.esc_for_velocity(goal_velocity  - error);
    }
  }
  return esc_ms;
}

/*
// todo: complete drive_route_2
void Driver::drive_route_2(Route & route) {

  double p_error;
  double i_error;
  double d_error;

  // while route not done
  // update route position

  // calculate desired curvature
  // d_error = (car.heading  - route.heading(current_point))*car.velocity
  // deisred_curvature = route.curvature(current_point, look_ahead) + kp * error / min(speed,1) + kd * d_error
}
*/

void Driver::drive_route(Route & route) {

  // we will set error text if something goes wrong
  string error_text = "";

  WorkQueue<Dynamics> queue;
  car.add_listener(&queue);
  ui.clear();
  ui.print("press any key to abort");
  ui.refresh();
  try {
    car.set_rc_mode();
    while(true) {
      if(ui.getkey()!=-1) {
        error_text = "run aborted by user";
        break;
      }

      Dynamics d;
      if(!queue.try_pop(d,1000)) {
        throw (string) "timed out waiting to read dynamics";
      }

      auto p_front = car.get_front_position();
      auto p_rear = car.get_rear_position();
      double v = car.get_velocity();
      double ahead = d_ahead + v*t_ahead;
      route.set_position(p_front, p_rear, v);

      //Angle steering_angle = steering_angle_by_look_ahead(route, ahead);
      Angle curvature = curvature_by_look_ahead(route,ahead);

      unsigned str = car.steering_for_curvature(curvature);
      unsigned esc = esc_for_velocity(route.get_velocity(), car);


      if(route.done && fabs(car.get_velocity()) < 0.05)
        break;
      car.set_esc_and_str(esc, str);
    }
  } catch (string s) {
    error_text = s;
  } catch (...) {
    error_text = "unknown exception caught during play_route";
  }

  car.set_esc_and_str(1500,1500);
  car.set_manual_mode();
  car.remove_listener(&queue);
  if(error_text.size() > 0) throw error_text;
}

Angle Driver::steering_angle_by_cte(Route &route) {
  double cte = route.cte;
  Angle segment_heading = route.heading();
  Angle car_heading = car.get_heading();
  Angle heading_fix = segment_heading - car_heading;
  double cte_fix = -80 * cte;
  if(car.get_velocity() < 0) {
    heading_fix = -heading_fix;
    cte_fix = -2*cte_fix;
  }
  Angle steering_angle = heading_fix + Angle::degrees(cte_fix);
  steering_angle.standardize();
  return steering_angle;
}

Angle Driver::curvature_by_look_ahead(Route &route, double look_ahead)
{
  double v = car.get_velocity();
  if(v<0)
    return steering_angle_by_cte(route);
  Point p = route.get_position_ahead(look_ahead).get_front_position();
  Point car_p = car.get_front_position();
  double d = distance(car_p,p);
  Angle heading = car.get_heading();
  Angle desired = Angle::radians(atan2(p.y-car_p.y,p.x-car_p.x));
  Angle delta = desired-heading;
  double ahead = d*cos(delta.radians());
  double left = d*sin(delta.radians());
  auto arc = car.ackerman.arc_to_relative_location(ahead, left);
  return Angle::radians(arc.curvature);
}


#include "fake_car.h"

void test_driver() {
    Car car(false);
    Route route;
    Dynamics d;
    car.apply_dynamics(d);
    car.wheelbase_length = 1;
    car.reset_odometry();

    RouteNode n;
    n.front_x = car.get_front_position().x;
    n.front_y = car.get_front_position().y;
    n.rear_x = car.get_rear_position().x;
    n.rear_y = car.get_rear_position().y;
    n.heading = car.get_heading().radians();
    n.reverse = false;

    // make route a 10x10 l shape
    route.add_node(n);
    n.front_x += 10;
    n.rear_x += 10;
    route.add_node(n);
    n.front_y = -10;
    n.rear_y = -10-car.wheelbase_length;
    route.add_node(n);
    route.optimize_velocity();
    cout << route.to_string() << endl;

    CarUI ui;
    RunSettings settings;
    Driver driver(car,ui,settings);
    for(auto look_ahead : linspace(0,25,1)){
        RouteNode n_ahead = route.get_position_ahead(look_ahead);
        cout << "look_ahead: " << look_ahead
             << " (" << n_ahead.front_x
             << " ," << n_ahead.front_y << ")"
             << " curvature: "
             << driver.curvature_by_look_ahead(route, look_ahead).degrees()
             << endl;
    }
}
