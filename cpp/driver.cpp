#include "driver.h"
#include "car.h"
#include "math.h"
#include "car_ui.h"
#include <unistd.h> // usleep
#include "logger.h"
#include "pid.h"


Driver::Driver(Car & car, CarUI& ui, RunSettings& settings)
  : car(car), ui(ui), settings(settings)
{
}


bool Driver::rear_slipping() {
  double v_front = (car.get_front_left_wheel().get_velocity(), car.get_front_right_wheel().get_velocity()) / 2.0;
  double v_back = (car.get_back_left_wheel().get_velocity(), car.get_back_right_wheel().get_velocity()) / 2.0;

  bool spinning = fabs(v_front - v_back) > (settings.slip_slop + (settings.slip_rate * v_front));
  return spinning;

}

int Driver::esc_for_max_decel() {
  int esc = 1500;
  double v_front = (car.get_front_left_wheel().get_velocity(), car.get_front_right_wheel().get_velocity()) / 2.0;
  double v_back = (car.get_back_left_wheel().get_velocity(), car.get_back_right_wheel().get_velocity()) / 2.0;
  if(v_front >= 0.1 && v_back >= v_front * (1.0-settings.slip_rate)) {
    esc = 1100;
  }
  if(v_front <= -0.1 && v_back <= v_front * (1.0-settings.slip_rate)) {
    esc = 1800;
  }
  return esc;
}

int Driver::esc_for_velocity(double goal_velocity, double goal_accel) {
  double velocity_output = goal_velocity + settings.v_k_p * (goal_velocity - car.get_velocity()) * settings.v_k_d * goal_accel;
  return car.esc_for_velocity(velocity_output/2);
}



void Driver::drive_route(Route & route) {
  log_info("entering drive_route");

  // we will set error text if something goes wrong
  string error_text = "";

  WorkQueue<Dynamics> queue;
  car.add_listener(&queue);
  ui.clear();
  ui.print("[abort]");
  ui.refresh();
  try {
    car.set_rc_mode();
    PID pid;
    pid.k_p = settings.steering_k_p;
    pid.k_i = settings.steering_k_i;
    pid.k_d = settings.steering_k_d;
    while(true) {
      if(ui.getkey()!=-1) {
        error_text = "run aborted by user";
        log_info("run aborted by user");
        break;
      }

      Dynamics d;
      if(!queue.try_pop(d,1000)) {
        log_error("timed out reading dynamics in drive_route");
        throw (string) "timed out waiting to read dynamics";
      }

      auto p_front = car.get_front_position();
      auto p_rear = car.get_rear_position();
      double v = car.get_velocity();
      double ahead = settings.d_ahead + v*settings.t_ahead;
      if(!route.done)
      route.set_position(p_front, p_rear, v);

      //Angle steering_angle = steering_angle_by_look_ahead(route, ahead);
      Angle track_curvature = curvature_by_look_ahead(route,ahead);

      // calculate derivitive term of the error
      /*
      Angle e_heading = car.get_heading() - route.get_heading_at_current_position();
      double d_error = sin(e_heading.radians())*v;
      Angle d_adjust = Angle::degrees(clamp(settings.steering_k_d * d_error,-30,30));


      Angle p_adjust = Angle::degrees(clamp(-1. * settings.steering_k_p * route.cte  / (v+1),-30,30));

      //
      */
      pid.add_reading((double)car.current_dynamics.us / 1E6, -route.cte);
      Angle pid_adjust = Angle::degrees(clamp(pid.output(),-60,60));
      Angle curvature = track_curvature + pid_adjust;



      unsigned str = route.done ? 1500 : car.steering_for_curvature(curvature);
      unsigned esc = route.done? esc_for_max_decel() : esc_for_velocity(route.get_velocity(), route.get_acceleration());
      if(rear_slipping())
        esc = 1500;

      if(route.done && fabs(car.get_velocity()) == 0.0) {
        log_info("route completed normally");
        break;
      }
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
  log_info("exiting drive_route");
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

