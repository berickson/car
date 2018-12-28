#include "driver.h"
#include <unistd.h>  // usleep
#include "car.h"
#include "logger.h"
#include "math.h"
#include "pid.h"
#include "stereo_camera.h"
#include "string_utils.h"

class VelocityTracker {
 public:
  double velocity_output = 0;
  double k_v = 1.0;
  double k_a = 0.0;
  double last_t = 0;
  double max_v_sp = 10;
  double min_v_sp = -10;

  void reset() {
    velocity_output = 0;
    last_t = 0;
  }

  int get_esc(Car & car, double v_sp, double a_sp, bool is_rear_slipping) {
    double t = car.get_time_in_seconds();
    if (last_t != 0) {
      double dt = t - last_t;
      // double v = car.get_velocity();
      double v = car.get_rear_velocity();
      double a = car.get_smooth_acceleration();
      double v_error = v_sp - v;
      double a_error = a_sp - a;

      velocity_output += (k_v * v_error) * dt;

      // if going along, trying to stop
      // velocity should be small and acceleration should be negative
      // problem is we are going backward at a constant velocity
      // the negative acceleration is just enough to counter the incorrect
      // direction don't consider acceleration at the end of the run or if going
      // the wrong direction or if we are at the end and want to be stationary
      bool going_correct_direction = ((v_sp > 0) == (v > 0));
      bool should_be_stopped = (v_sp == 0 && a_sp == 0);
      if (going_correct_direction && !should_be_stopped)
        velocity_output += (k_a * a_error) * dt;

      velocity_output = clamp(velocity_output, min_v_sp, max_v_sp);
    }
    last_t = t;
    return is_rear_slipping ? 1500 : car.esc_for_velocity(velocity_output);
  }

  int get_esc(Route & route, Car & car, bool is_rear_slipping) {
    if (is_rear_slipping)
      return 1500;

    return get_esc(
        car, route.get_velocity(), route.get_acceleration(), is_rear_slipping);
  }
};

VelocityTracker velocity_tracker;

Driver::Driver(Car & car, RunSettings & settings)
    : car(car), settings(settings) {}

// returns true if crashed
bool Driver::check_for_crash() {
  ::Point position = car.get_front_position();
  unsigned int ms = car.current_dynamics.ms;

  // if we don't have a checkpoint, get one and return
  if (crash_checkpoint.valid == false) {
    crash_checkpoint.ms = car.current_dynamics.ms;
    crash_checkpoint.position = position;
    crash_checkpoint.valid = true;
    return false;
  }

  // if car has moved more than 0.05 meters, replace checkpoint
  if (distance(position, crash_checkpoint.position) > 0.05) {
    crash_checkpoint.ms = ms;
    crash_checkpoint.position = position;
    crash_checkpoint.valid = true;
    return false;
  }

  // we've crashed if more than 3 seconds have elapsed since last checkpoint
  if (ms - crash_checkpoint.ms > 3000) {
    // set last_crash_location to location
    previous_crash = current_crash;  // remember the crash location
    current_crash = crash_checkpoint;
    current_crash.ms = ms;
    return true;
  }
  return false;
}

void Driver::avoid_barrels(StereoCamera & camera) {
  log_entry_exit w("avoid barrels");
  int str = 1500;
  double v_sp = 0.25;
  double a_sp = 0;
  string direction = camera.get_clear_driving_direction();
  log_info("steering direction: " + direction);
  if (direction == "left") {
    // steer left
    str = car.steering_for_angle(Angle::degrees(20));
  }
  if (direction == "right") {
    // steer right
    str = car.steering_for_angle(Angle::degrees(-20));
  }
  if (direction == "straight") {
    str = car.steering_for_angle(Angle::degrees(0));
  }
  if (direction == "unknown") {
    str = car.steering_for_angle(Angle::degrees(0));
    v_sp = 0;
  }
  int esc = velocity_tracker.get_esc(car, v_sp, a_sp, rear_slipping());
  car.set_esc_and_str(esc, str);
}

bool Driver::rear_slipping() {
  return false;
  double v_front = car.get_velocity();
  double v_back = 0.5 *
      (car.get_back_left_wheel().get_velocity() +
          car.get_back_right_wheel().get_velocity());

  bool spinning = fabs(v_front - v_back) >
      (settings.slip_slop + (settings.slip_rate * fabs(v_front)));
  return spinning;
}

int Driver::esc_for_max_decel() {
  int esc = 1500;
  double v_front = car.get_velocity();
  double v_back = (car.get_back_left_wheel().get_smooth_velocity(),
                      car.get_back_right_wheel().get_smooth_velocity()) /
      2.0;
  if (v_front >= 0.1 && v_back >= v_front * (1.0 - settings.slip_rate)) {
    esc = 1100;
  }
  if (v_front <= -0.1 && v_back <= v_front * (1.0 - settings.slip_rate)) {
    esc = 1800;
  }
  return esc;
}

int Driver::esc_for_velocity(
    PID & velocity_pid, double goal_velocity, double goal_accel) {
  double v = car.get_velocity();
  double t = (double)car.current_dynamics.us / 1E6;
  // if(fabs(v) > 1.2 * fabs(goal_velocity) && fabs(v) > 0.5) {
  //  return esc_for_max_decel();
  //}
  float v_error = goal_velocity - v;
  velocity_pid.add_reading(t, v_error);
  double velocity_output =
      goal_velocity + velocity_pid.output() + settings.v_k_d * goal_accel;
  return car.esc_for_velocity(velocity_output);
}

void Driver::continue_along_route(Route & route, StereoCamera & camera) {
  auto p_front = car.get_front_position();
  auto p_rear = car.get_rear_position();
  double v = car.get_velocity();
  double ahead = settings.d_ahead + v * settings.t_ahead;
  if (!route.done) {
    unsigned int old_index = route.index;
    route.set_position(p_front, p_rear, v);
    unsigned int new_index = route.index;
    for (unsigned int i = old_index + 1; i <= new_index; i++) {
      if (route.nodes[i].road_sign_command == "beep") {
        log_info("car commanded to beep");
        car.beep();
      }
      if (route.nodes[i].road_sign_command == "avoid_barrels") {
        log_info("car commanded to avoid barrels");
        mode = "avoid_barrels";
        camera.process_disparities_enabled = true;
      }
    }
  }

  // Angle steering_angle = steering_angle_by_look_ahead(route, ahead);
  Angle e_heading = route.get_heading_at_current_position() - car.get_heading();
  double d_error = sin(e_heading.radians());

  Angle d_adjust =
      Angle::degrees(clamp(settings.steering_k_d * d_error, -20, 20));
  Angle p_adjust = Angle::degrees(
      clamp(settings.steering_k_p * route.cte / (v + 1), -20, 20));
  Angle curvature = required_turn_curvature_by_look_ahead(route, ahead);

  unsigned str = route.done
      ? 1500
      : car.steering_for_curvature(curvature) + p_adjust + d_adjust;
  // unsigned esc = route.done? esc_for_velocity(velocity_pid, 0, 0) :
  // esc_for_velocity(velocity_pid, route.get_velocity(),
  // route.get_acceleration());
  unsigned esc = velocity_tracker.get_esc(route, car, rear_slipping());

  if (route.done && fabs(car.get_velocity()) < 0.01) {
    log_info("route completed normally");
    route_complete = true;
    esc = 1500;
    str = 1500;
  }
  car.set_esc_and_str(esc, str);
}

// tries to stop at stop_node
// returns true if done
bool Driver::continue_to_stop(Route & route, StereoCamera & camera) {
  continue_along_route(route, camera);
  return fabs(car.get_velocity()) < 0.01;
}

void Driver::set_evasive_actions_for_crash(Route & route) {
  Point correction;  // meters relative to car update car state

  // if this is a repeat crash, try something new
  bool repeated_crash = previous_crash.valid &&
      distance(current_crash.position, previous_crash.position) < 0.1;
  if (repeated_crash) {
    log_warning("detected a repeated crash, trying something else");
    correction.x =
        -last_crash_correction.x;  // x correction was probably wrong, remove it
    if (fabs(last_crash_correction.y) < 0.1) {
      correction.y = 1.0;
    } else if (last_crash_correction.y > 0) {
      correction.y = -fabs(last_crash_correction.y) - 1.0;
    } else {
      // last must be negative
      correction.y = fabs(last_crash_correction.y) + 1.0;
    }

    correction.x = 1.0;
  } else {
    // new crash, try to guess what to do
    Angle error_angle =
        car.get_heading() - route.get_heading_at_current_position();

    log_warning(
        (string) "angle error at crash " + format(error_angle.degrees(), 5, 1));
    // when it his a wall obliquely, the car tends to turn toward the wall,
    // the goal will shift to the opposite side of the car from the wall,
    // the car is too far to the position opposite the goal, change position
    // accordingly
    if (fabs(error_angle.degrees() < 20.)) {
      log_info(
          "looks like a head on collision, assuming position too far forward");
      correction.x = 1.0;
    } else if (error_angle.degrees() > 0) {
      log_info(
          "appeared to glance left, wall must be to right, assuming position "
          "too far right");
      correction.y = -1.0;
    } else {
      log_info(
          "appeared to glance right, wall must be to left, assuming position "
          "too far left");
      correction.y = 1.0;
    }
  }
  log_warning((string) "crashed, changing position by " +
      correction.to_string() + " meters");

  // update car and crash position with correction
  car.ackerman.move_relative_to_heading(correction);
  current_crash.position = car.ackerman.front_position();

  last_crash_correction = correction;
}

// returns distance to nearest obstacle within safe
// stopping range ahead of car.
//
// returns NAN if no obstacle or car going in reverse
double nearest_obstacle_distance_on_route(const Car & car, const Route & route,
    const RunSettings & settings, double stop_margin) {
  // meters between car positions to check
  double check_increment = 0.1;

  // meters air cushion to have between car and obstacles
  double air_cushion = 0.0;

  const auto & scan = car.lidar.current_scan;
  double v = car.get_velocity();

  // only check in front for now
  if (v < 0) {
    return NAN;
  }

  double max_decel = settings.max_decel;
  double decel_time = v / max_decel;
  double safe_distance =
      0.5 * max_decel * decel_time * decel_time + stop_margin + 1;

  // get vectors for path and theta to check

  vector<double> path_x;
  vector<double> path_y;
  vector<double> path_theta;
  vector<double> path_d;

  for (double ahead = 0; ahead < safe_distance + check_increment;
       ahead += check_increment) {
    RouteNode position =
        route.get_position_ahead(ahead);  // todo: check for effiency
    path_d.push_back(ahead);
    path_x.push_back(position.rear_x);
    path_y.push_back(position.rear_y);
    path_theta.push_back(Angle::degrees(position.heading).radians());
  }

  // get vectors for lidar
  vector<double> lidar_world_x;
  vector<double> lidar_world_y;
  assert(scan.measurements.size() == scan.poses.size());
  for (size_t i = 0; i < scan.measurements.size(); i++) {
    const LidarMeasurement & measurement = scan.measurements[i];
    auto l = measurement.distance_meters;
    if (isnan(l)) {
      continue;
    }
    Pose2dSimple pose_simple = scan.poses[i];
    Pose2d pose(
        Angle::radians(pose_simple.theta), {pose_simple.x, pose_simple.y});

    auto pose_to_world = Transform2d::pose_to_world_transform(pose);
    auto theta = measurement.angle.radians();
    auto x = cos(theta) * l;
    auto y = sin(theta) * l;
    Point p(x, y);
    Point point_world = pose_to_world(p);

    lidar_world_x.emplace_back(point_world.x);
    lidar_world_y.emplace_back(point_world.y);
  }

  // get car shape
  double l = car.wheelbase_length;
  double w = car.front_wheelbase_width;
  vector<double> car_shape_x{0, 0, l, l, 0};
  vector<double> car_shape_y{w / 2, -w / 2, -w / 2, w / 2, w / 2};

  // get intersections
  vector<size_t> intersections =
      lidar_path_intersections(path_x, path_y, path_theta, lidar_world_x,
          lidar_world_y, car_shape_x, car_shape_y, air_cushion);

  // return closest
  if (intersections.size() == 0) {
    return NAN;
  }
  return path_d[intersections[0]];
}

void Driver::drive_route(Route & route, StereoCamera & camera) {
  log_entry_exit w("drive_route");
  route.nodes[route.nodes.size() - 1].road_sign_command = "stop";

  // we will set error text if something goes wrong
  string error_text = "";

  WorkQueue<Dynamics2> queue(1);  // limit to 1 to only get latest
  car.add_listener(&queue);

  try {
    car.set_rc_mode();
    PID steering_pid;
    steering_pid.k_p = settings.steering_k_p;
    steering_pid.k_i = settings.steering_k_i;
    steering_pid.k_d = settings.steering_k_d;

    PID velocity_pid;
    double voltage = car.get_voltage();
    double voltage_factor =
        (voltage != NAN && voltage > 0) ? 100 / (voltage * voltage) : 1.0;
    velocity_pid.k_p = settings.v_k_p * voltage_factor;
    velocity_pid.k_i = settings.v_k_i * voltage_factor;
    velocity_pid.k_d = settings.v_k_d * voltage_factor;

    velocity_tracker.k_v = settings.v_k_p * voltage_factor;
    velocity_tracker.k_a = settings.v_k_d * voltage_factor;
    velocity_tracker.reset();

    route_complete = false;
    recovering_from_crash = false;
    // int crash_count = 0;
    system_clock::time_point wait_end_time = system_clock::now();
    mode = "follow_route";
    while (!route_complete) {
      Dynamics2 d;
      if (!queue.try_pop(d, 1000)) {
        log_error("timed out reading dynamics in drive_route");
        throw(string) "timed out waiting to read dynamics";
      }

      double stop_margin = 1;
      double obstacle_distance =
          nearest_obstacle_distance_on_route(car, route, settings, stop_margin);
      if (isnan(obstacle_distance)) {
        ;  // log_info((string)"No obstacle detected");
      } else {
        log_info((string) "Nearest obstacle detected at distance " +
            to_string(obstacle_distance) + " meters.");
      }

      // double obstacle_distance = NAN;
      Route * chosen_route = &route;
      Route short_term_route;
      if (!isnan(obstacle_distance)) {
        double step = 0.05;
        double total_ahead = obstacle_distance + step + stop_margin + 1;

        // one meter should ensure route is never empty
        total_ahead = max<double>(total_ahead, 1.);

        for (double ahead = 0; ahead < total_ahead; ahead += step) {
          RouteNode n = route.get_position_ahead(ahead);
          double obstacle_v = velocity_at_position(
              obstacle_distance - ahead - stop_margin, settings.max_decel, 0);
          if (isnan(obstacle_v)) {
            obstacle_v = 0;
          }
          obstacle_v = max(obstacle_v, 0.0);
          double obstacle_a = -settings.max_decel;
          n.velocity = min(n.velocity, obstacle_v);
          if (n.velocity == 0) {
            n.road_sign_command = "stop";
            n.road_sign_label = "temporary stop";
          }
          short_term_route.add_node(n);
        }
        short_term_route.set_position(car.get_front_position(),
            car.get_rear_position(), car.get_velocity());
        // avoid invalid routes
        if (short_term_route.nodes.size() > 1) {
          chosen_route = &short_term_route;
        }
        // log_info(short_term_route.to_string());
      }

      // execute route flow, based on
      // https://docs.google.com/drawings/d/1S2gPPzPD42xvuomWY12pHNqIRDZXRV040nHIy7_USxc/edit?usp=sharing

      if (mode == "avoid_barrels") {
        RouteNode * avoid_start_node = route.get_source_node();
        double avoid_meters = stod(avoid_start_node->arg1);
        double distance_travelled = distance(
            car.get_front_position(), avoid_start_node->get_front_position());
        if (distance_travelled < avoid_meters) {
          avoid_barrels(camera);
        } else {
          mode = "follow_route";
          velocity_tracker.reset();  // otherwise, excessive accel after waiting
          log_info("done waiting");
        }
      }

      if (mode == "follow_route") {
        continue_along_route(*chosen_route, camera);

        if (chosen_route->is_stop_ahead()) {
          mode = "stop_at_point";
          log_info("stopping");
        }
      }

      if (mode == "stop_at_point") {
        RouteNode * stop_node = chosen_route->get_target_node();
        bool stop_complete = continue_to_stop(*chosen_route, camera);
        if (stop_complete) {
          chosen_route->advance_to_next_segment();
          if (route.done) {
            mode = "done";
            route_complete = true;
          } else {
            mode = "wait";
            double wait_seconds =
                stop_node->arg1.length() > 0 ? stod(stop_node->arg1) : 1;
            wait_end_time =
                system_clock::now() + milliseconds((long)(wait_seconds * 1000));
            log_info("waiting");
          }
        }
      }

      if (mode == "wait") {
        car.set_esc_and_str(1500, 1500);
        if (system_clock::now() > wait_end_time) {
          mode = "follow_route";
          velocity_tracker.reset();  // otherwise, excessive accel after waiting
          log_info("done waiting");
        }
      }

      if (mode == "done") {
        car.set_esc_and_str(1500, 1500);
        car.set_manual_mode();
      }

      if (settings.crash_recovery == true && check_for_crash()) {
        log_warning("crash detected, aborting run");
        route_complete = true;
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
  if (error_text.length() > 0) {
    log_error(error_text);
  }

  car.set_manual_mode();
  car.remove_listener(&queue);
  if (error_text.size() > 0)
    throw error_text;
}

Angle Driver::steering_angle_by_cte(Route & route) {
  double cte = route.cte;
  Angle segment_heading = route.heading();
  Angle car_heading = car.get_heading();
  Angle heading_fix = segment_heading - car_heading;
  double cte_fix = 80 * cte;
  if (car.get_velocity() < 0) {
    heading_fix = -heading_fix;
    cte_fix = -2 * cte_fix;
  }
  Angle steering_angle = heading_fix + Angle::degrees(cte_fix);
  steering_angle.standardize();
  return steering_angle;
}

Angle Driver::required_turn_curvature_by_look_ahead(
    Route & route, double look_ahead) {
  double v = car.get_velocity();
  if (v < 0)
    return steering_angle_by_cte(route);
  Point p = route.get_position_ahead(look_ahead).get_front_position();
  Point car_p = car.get_front_position();
  double d = distance(car_p, p);
  Angle heading = car.get_heading();
  Angle desired = Angle::radians(atan2(p.y - car_p.y, p.x - car_p.x));
  Angle delta = desired - heading;
  double ahead = d * cos(delta.radians());
  double left = d * sin(delta.radians());
  auto arc = car.ackerman.arc_to_relative_location(ahead, left);
  return Angle::radians(arc.curvature);
}

#include "fake_car.h"

void test_driver() {
  Car car(false);
  Route route;
  Dynamics2 d;
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
  n.rear_y = -10 - car.wheelbase_length;
  route.add_node(n);
  route.optimize_velocity();
  cout << route.to_string() << endl;

  RunSettings settings;
  Driver driver(car, settings);
  for (auto look_ahead : linspace(0, 25, 1)) {
    RouteNode n_ahead = route.get_position_ahead(look_ahead);
    cout << "look_ahead: " << look_ahead << " (" << n_ahead.front_x << " ,"
         << n_ahead.front_y << ")"
         << " curvature: "
         << driver.required_turn_curvature_by_look_ahead(route, look_ahead)
                .degrees()
         << endl;
  }
}
