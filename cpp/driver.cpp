#include "driver.h"
#include "car.h"
#include "math.h"
#include "car_ui.h"
#include <unistd.h> // usleep
#include "logger.h"
#include "pid.h"
#include "string_utils.h"


Driver::Driver(Car & car, RunSettings& settings)
  : car(car), settings(settings)
{
}

// returns true if crashed
bool Driver::check_for_crash() {
  Point position = car.get_front_position();
  unsigned int ms = car.current_dynamics.ms;

  // if we don't have a checkpoint, get one and return
  if(crash_checkpoint.valid == false) {
    crash_checkpoint.ms = car.current_dynamics.ms;
    crash_checkpoint.position = position;
    crash_checkpoint.valid = true;
    return false;
  }

  // if car has moved more than 0.05 meters, replace checkpoint
  if(distance(position,crash_checkpoint.position) > 0.05) {
    crash_checkpoint.ms = ms;
    crash_checkpoint.position = position;
    crash_checkpoint.valid = true;
    return false;
  }

  // we've crashed if more than 3 seconds have elapsed since last checkpoint
  if(ms-crash_checkpoint.ms > 3000) {
    // set last_crash_location to location
    previous_crash = current_crash; // remember the crash location
    current_crash = crash_checkpoint;
    current_crash.ms = ms;
    return true;
  }
  return false;
}


bool Driver::rear_slipping() {
  double v_front = car.get_velocity();
  double v_back = (car.get_back_left_wheel().get_smooth_velocity(), car.get_back_right_wheel().get_smooth_velocity()) / 2.0;

  bool spinning = fabs(v_front - v_back) > (settings.slip_slop + (settings.slip_rate * v_front));
  return spinning;

}

int Driver::esc_for_max_decel() {
  int esc = 1500;
  double v_front = car.get_velocity();
  double v_back = (car.get_back_left_wheel().get_smooth_velocity(), car.get_back_right_wheel().get_smooth_velocity()) / 2.0;
  if(v_front >= 0.1 && v_back >= v_front * (1.0-settings.slip_rate)) {
    esc = 1100;
  }
  if(v_front <= -0.1 && v_back <= v_front * (1.0-settings.slip_rate)) {
    esc = 1800;
  }
  return esc;
}

int Driver::esc_for_velocity(PID & velocity_pid, double goal_velocity, double goal_accel) {
  double v = car.get_velocity();
  double t = (double)car.current_dynamics.us / 1E6;
  //if(fabs(v) > 1.2 * fabs(goal_velocity) && fabs(v) > 0.5) {
  //  return esc_for_max_decel();
  //}
  float v_error = goal_velocity - v;
  velocity_pid.add_reading(t, v_error);
  double velocity_output = goal_velocity + velocity_pid.output() + settings.v_k_d * goal_accel;
  return car.esc_for_velocity(velocity_output);
}


class VelocityTracker {
public:
  double velocity_output = 0;
  double k_v = 10000.0;
  double k_a = 0.0;
  double last_t = 0;
  double max_v_sp = 10;
  double min_v_sp = -10;

  void reset() {
    velocity_output = 0;
    last_t = 0;
  }

  int get_esc(Route &  route, Car & car) {
    double t = car.get_time_in_seconds();
    if (last_t != 0) {
      double dt = t - last_t;
      double v = car.get_rear_velocity();
      double a = car.get_smooth_acceleration();
      double v_sp = route.get_velocity();
      double a_sp = route.get_acceleration();
      double v_error = v_sp - v;
      double a_error = a_sp - a;

      velocity_output += (k_v * v_error) * dt;
      
      // don't consider acceleration at the end of the run
      if(v_sp != 0 || a_sp != 0)
        velocity_output += (k_a * a_error) * dt;

      velocity_output = clamp(velocity_output, min_v_sp, max_v_sp);
      //stringstream ss;
      //ss.precision(1);
      //ss <<  "rt.v:" << route.get_velocity() << "  car.v:" << v;
      //ss <<  "rt.a:" << route.get_acceleration() << "  car.a:" << a << "  velocity_output:" << velocity_output;
      //log_info(ss.str());
    }
   last_t = t;
   return car.esc_for_velocity(velocity_output);
  }
};

VelocityTracker velocity_tracker;

void Driver::continue_along_route(Route& route, PID& steering_pid, PID& velocity_pid)
{
  auto p_front = car.get_front_position();
  auto p_rear = car.get_rear_position();
  double v = car.get_velocity();
  double ahead = settings.d_ahead + v*settings.t_ahead;
  if(!route.done) {
    unsigned int old_index = route.index;
    route.set_position(p_front, p_rear, v);
    unsigned int new_index = route.index;
    for(unsigned int i = old_index + 1; i <= new_index; i++) {
      if(route.nodes[i].road_sign_command == "beep") {
        log_info("car commanded to beep");
        car.beep();
      }
    }
  }

  // Angle steering_angle = steering_angle_by_look_ahead(route, ahead);
  Angle e_heading = route.get_heading_at_current_position() - car.get_heading();
  double d_error = sin(e_heading.radians());

  Angle d_adjust = Angle::degrees(clamp(settings.steering_k_d * d_error,-20,20));
  Angle p_adjust = Angle::degrees(clamp(settings.steering_k_p * route.cte  / (v+1),-20,20));
  Angle curvature = required_turn_curvature_by_look_ahead(route, ahead);



  unsigned str = route.done ? 1500 : car.steering_for_curvature(curvature) + p_adjust + d_adjust;
  //unsigned esc = route.done? esc_for_velocity(velocity_pid, 0, 0) : esc_for_velocity(velocity_pid, route.get_velocity(), route.get_acceleration());
  unsigned esc = velocity_tracker.get_esc(route, car);

  if(rear_slipping() && 0)
    esc = 1500;

  if(route.done && fabs(car.get_velocity()) < 0.01) {
    log_info("route completed normally");
    route_complete = true;
    esc = 1500;
    str = 1500;
  }
  car.set_esc_and_str(esc, str);
}

// tries to stop at stop_node
// returns true if done
bool Driver::continue_to_stop(const RouteNode *stop_node)
{
  car.set_esc_and_str(1500,1500);
  return fabs(car.get_velocity()) < 0.01;
}

void Driver::set_evasive_actions_for_crash(Route& route)
{
  Point correction; // meters relative to car update car state

  // if this is a repeat crash, try something new
  bool repeated_crash = previous_crash.valid && distance(current_crash.position,previous_crash.position) < 0.1;
  if( repeated_crash ) {
    log_warning("detected a repeated crash, trying something else");
    correction.x = -last_crash_correction.x; // x correction was probably wrong, remove it
    if(fabs(last_crash_correction.y) <0.1) {
      correction.y = 1.0;
    }
    else if (last_crash_correction.y > 0) {
      correction.y = - fabs(last_crash_correction.y) - 1.0;
    }
    else {
      // last must be negative
      correction.y = fabs(last_crash_correction.y) + 1.0;
    }

    correction.x = 1.0;
  } else {
    // new crash, try to guess what to do
    Angle error_angle = car.get_heading()-route.get_heading_at_current_position();


    log_warning((string) "angle error at crash " + format(error_angle.degrees(),5,1));
    // when it his a wall obliquely, the car tends to turn toward the wall,
    // the goal will shift to the opposite side of the car from the wall,
    // the car is too far to the position opposite the goal, change position accordingly
    if(fabs(error_angle.degrees()<20.)) {
      log_info("looks like a head on collision, assuming position too far forward");
      correction.x = 1.0;
    } else if (error_angle.degrees() > 0) {
      log_info("appeared to glance left, wall must be to right, assuming position too far right");
      correction.y = -1.0;
    }
    else {
      log_info("appeared to glance right, wall must be to left, assuming position too far left");
      correction.y = 1.0;
    }
  }
  log_warning((string) "crashed, changing position by " + correction.to_string() + " meters");

  // update car and crash position with correction
  car.ackerman.move_relative_to_heading(correction);
  current_crash.position = car.ackerman.front_position();

  last_crash_correction = correction;
}

void Driver::drive_route(Route & route) {
  log_entry_exit w("drive_route");

  // we will set error text if something goes wrong
  string error_text = "";

  WorkQueue<Dynamics> queue;
  car.add_listener(&queue);

  try {
    car.set_rc_mode();
    PID steering_pid;
    steering_pid.k_p = settings.steering_k_p;
    steering_pid.k_i = settings.steering_k_i;
    steering_pid.k_d = settings.steering_k_d;

    PID velocity_pid;
    velocity_pid.k_p = settings.v_k_p;
    velocity_pid.k_i = settings.v_k_i;
    velocity_pid.k_d = settings.v_k_d;

    velocity_tracker.k_v = settings.v_k_p;
    velocity_tracker.k_a = settings.v_k_d;
    velocity_tracker.reset();

    route_complete = false;
    recovering_from_crash = false;
    int crash_count = 0;
    system_clock::time_point wait_end_time = system_clock::now();
    string mode = "follow_route";
    while(!route_complete) {
      Dynamics d;
      if(!queue.try_pop(d,1000)) {
        log_error("timed out reading dynamics in drive_route");
        throw (string) "timed out waiting to read dynamics";
      }
      // execute route flow, based on
      // https://docs.google.com/drawings/d/1S2gPPzPD42xvuomWY12pHNqIRDZXRV040nHIy7_USxc/edit?usp=sharing

      if(mode == "follow_route") {
        continue_along_route(route, steering_pid, velocity_pid);
        if(route.is_stop_ahead()) {
          mode = "stop_at_point";
          log_info("stopping");
        }
      }

      if(mode=="stop_at_point") {
        RouteNode * stop_node = route.get_target_node();
        // todo: try best to stop at the given point
        bool stop_complete  = continue_to_stop(stop_node);
        if (stop_complete) {
          route.advance_to_next_segment();
          if(route.done) {
            mode = "done";
            route_complete = true;
          } else {
            mode = "wait";
            double wait_seconds = stod(stop_node->arg1);
            wait_end_time = system_clock::now() + milliseconds((long)(wait_seconds * 1000));
            log_info("waiting");
          }
        }
      }

      if(mode == "wait") {
        car.set_esc_and_str(1500,1500);
        if(system_clock::now() > wait_end_time) {
          mode = "follow_route";
          log_info("done waiting");
        }

      }

      if(mode == "done") {
        car.set_esc_and_str(1500,1500);
        car.set_manual_mode();
      }
      /*

      if(settings.crash_recovery == true && !recovering_from_crash) {
        if(check_for_crash()) {
          log_warning("crash detected");
          recovering_from_crash = true;

          // repeat max of 20 times
          ++crash_count;
          if(crash_count >= 20) {
            route_complete = true;
            break;
          }
          // after a crash, set a plan for crash evasion
          set_evasive_actions_for_crash(route);
        }
      }


      if(recovering_from_crash) {
        // go backward for 1 meters at 1 m/s
        if(distance(car.get_front_position(),current_crash.position) > 1.0 ) {
          recovering_from_crash = false;
          car.set_esc_and_str(1500,1500);
          log_info("done backing away from crash");
        } else if (d.ms - current_crash.ms > 5000) {
          recovering_from_crash = false;
          car.set_esc_and_str(1500,1500);
          log_info("timed out trying to back away from crash");
        }
        else {
          car.set_esc_and_str(esc_for_velocity(velocity_pid, -3.0,0),1500);
        }
      } else {
        continue_along_route(route, steering_pid, velocity_pid);
      }
      */
    }
  } catch (string s) {
    error_text = s;
  } catch (...) {
    error_text = "unknown exception caught during play_route";
  }
  if(error_text.length() > 0) {
    log_error(error_text);
  }

  car.set_manual_mode();
  car.remove_listener(&queue);
  if(error_text.size() > 0) throw error_text;
}

Angle Driver::steering_angle_by_cte(Route &route) {
  double cte = route.cte;
  Angle segment_heading = route.heading();
  Angle car_heading = car.get_heading();
  Angle heading_fix = segment_heading - car_heading;
  double cte_fix = 80 * cte;
  if(car.get_velocity() < 0) {
    heading_fix = -heading_fix;
    cte_fix = -2*cte_fix;
  }
  Angle steering_angle = heading_fix + Angle::degrees(cte_fix);
  steering_angle.standardize();
  return steering_angle;
}

Angle Driver::required_turn_curvature_by_look_ahead(Route &route, double look_ahead)
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

    RunSettings settings;
    Driver driver(car,settings);
    for(auto look_ahead : linspace(0,25,1)){
        RouteNode n_ahead = route.get_position_ahead(look_ahead);
        cout << "look_ahead: " << look_ahead
             << " (" << n_ahead.front_x
             << " ," << n_ahead.front_y << ")"
             << " curvature: "
             << driver.required_turn_curvature_by_look_ahead(route, look_ahead).degrees()
             << endl;
    }
}

