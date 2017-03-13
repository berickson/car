#include "car.h"
#include "config.h"

#include <iostream>
#include "lookup_table.h"
#include "split.h"
#include "logger.h"

using namespace std;




Car::Car(bool online) {
  log_info("reading configuration");
  read_configuration(config_path);
  front_right_wheel.meters_per_tick = this->meters_per_odometer_tick;
  front_left_wheel.meters_per_tick = this->meters_per_odometer_tick;
  back_left_wheel.meters_per_tick = this->rear_meters_per_odometer_tick;
  back_right_wheel.meters_per_tick = this->rear_meters_per_odometer_tick;
  log_info("all wheels set");
  this->online = online;
  reset_odometry();
  if(online) {
    connect_usb();
  }
}

Car::~Car() {
  quit=true;
  if(usb_thread.joinable())
    usb_thread.join();
}

void Car::connect_usb() {
  usb_thread = thread(&Car::usb_thread_start, this);
}

void Car::usb_thread_start() {
  usb.add_line_listener(&usb_queue);
  usb.write_on_connect("\ntd+\n");
  usb.run();
  while(!quit) {
    string line;
    if(usb_queue.try_pop(line,1)) {
      process_line_from_log(line);
    }
  }
}

// returns true if the line is a valid TD
bool Car::process_line_from_log(string line) {
  if(input_recording_file.is_open()) {
    input_recording_file << line << endl; //todo: make non-blocking
  }
  if(split(line)[1]!="TD") {
    return false;
  }
  Dynamics d;
  bool ok = Dynamics::from_log_string(d,line);
  if(ok) {
    apply_dynamics(d);
    {
      lock_guard<mutex> lock(listeners_mutex);
      for(auto listener:listeners) {
        listener->push(d);
      }
    }
  }
  else {
    ++usb_error_count;
    cerr << "dynamics not ok for " << line << endl;
  }
  return ok;
}

void Car::send_command(string command) {
  usb.write_line(command);
}

void Car::set_rc_mode() {
  send_command("rc");

}

void Car::set_manual_mode() {
  send_command("m");
}

void Car::set_esc_and_str(unsigned esc, unsigned str)
{
  send_command((string)"pse "+to_string(str)+","+to_string(esc));
  commanded_esc = esc;
  commanded_str = str;
}


void Car::begin_recording_state(string path) {
  end_recording_state();
  state_recording_file.open(path, ios_base::out);
  state_recording_file << Dynamics::csv_field_headers();
  state_recording_file << ",v_smooth,v_fl_smooth,v_fr_smooth,v_bl_smooth,v_br_smooth,commanded_esc,commanded_str" << endl;
}

void Car::write_state() {
  if(state_recording_file.is_open()) {
    state_recording_file << current_dynamics.csv_fields() << ","
        << get_velocity() << ","
        << front_left_wheel.get_smooth_velocity() << ","
        << front_right_wheel.get_smooth_velocity() << ","
        << back_left_wheel.get_smooth_velocity() << ","
        << back_right_wheel.get_smooth_velocity() << ","
        << commanded_esc << ","
        << commanded_str << endl;
  }
}

void Car::end_recording_state() {
  if(state_recording_file.is_open()) {
    state_recording_file.flush();
    state_recording_file.close();
  }
}

void Car::begin_recording_input(string path) {
  end_recording_input();
  input_recording_file.open(path, ios_base::out);
}

void Car::end_recording_input() {
  if(input_recording_file.is_open()) {
    input_recording_file.flush();
    input_recording_file.close();
  }
}

void Car::apply_dynamics(Dynamics & d) {
  // set all the dynamics variables
  reading_count++;

  Dynamics previous = current_dynamics;
  current_dynamics = d;
  if(reading_count == 1) {
    original_dynamics = d;
    ackerman = Ackerman(front_wheelbase_width, wheelbase_length, Point(0,0), get_heading()); // ackerman needs first heading reading
    return;
  }

  // update state
  Dynamics & current = current_dynamics;

  // correct heading with adjustment factor
  Angle d_theta = (current.yaw - previous.yaw);
  d_theta.standardize();
  //heading_adjustment += Angle::radians(d_theta.radians() * gyro_adjustment_factor);

  // if wheels have moved, update ackerman
  back_left_wheel.update_from_sensor(current.us, current.odometer_back_left_last_us, current.odometer_back_left, current.ax);
  back_right_wheel.update_from_sensor(current.us, current.odometer_back_right_last_us, current.odometer_back_right, current.ax);
  front_left_wheel.update_from_sensor(current.us, current.odometer_front_left_last_us, current.odometer_front_left, current.ax);
  double wheel_distance_meters = front_right_wheel.update_from_sensor(current.us, current.odometer_front_right_last_us, current.odometer_front_right, current.ax);
  if (reading_count > 2 && fabs(wheel_distance_meters) > 0.) { // adding 2 keeps out the big jump after a reset
    Angle outside_wheel_angle = angle_for_steering(previous.str);
    ackerman.move_right_wheel(outside_wheel_angle, wheel_distance_meters, get_heading().radians());
  }
  write_state();
}

void Car::add_listener(WorkQueue<Dynamics>* listener) {
  lock_guard<mutex> lock(listeners_mutex);
  listeners.push_back(listener);
}

void Car::remove_listener(WorkQueue<Dynamics>*listener) {
  lock_guard<mutex> lock(listeners_mutex);
  listeners.remove(listener);
}

void Car::read_configuration(string path){
  Config config;
  config.load_from_file(path);

  // odometery
  meters_per_odometer_tick = config.get_double("meters_per_odometer_tick");
  rear_meters_per_odometer_tick = config.get_double("rear_meters_per_odometer_tick");
  gyro_adjustment_factor = config.get_double("gyro_adjustment_factor");

  // esc and steering
  center_steering_us = config.get_double("center_steering_us");
  min_forward_esc = config.get_int("min_forward_esc");
  min_reverse_esc = config.get_int("min_reverse_esc");
  reverse_center_steering_us = config.get_int("reverse_center_steering_us");

  // car dimensions
  front_wheelbase_width = config.get_double("front_wheelbase_width_in_meters");
  rear_wheelbase_width = config.get_double("rear_wheelbase_width_in_meters");
  wheelbase_length = config.get_double("wheelbase_length_in_meters");
}

void Car::reset_odometry() {
  //dynamics = Dynamics();
  original_dynamics = current_dynamics;

  set_zero_heading();

  ackerman = Ackerman(
    front_wheelbase_width,
    wheelbase_length,Point(0,0),Angle::degrees(0));
}

void Car::set_zero_heading() {
  zero_heading_angle = current_dynamics.yaw;
}

Angle Car::get_zero_heading() {
  return zero_heading_angle;
}

Angle Car::get_heading() {
  Angle a = current_dynamics.yaw - zero_heading_angle;
  a.standardize();
  return a;
}


int Car::steering_for_curvature(Angle theta_per_meter) {
  static const LookupTable t(
  {
    {-85.1, 1929},
    {-71.9, 1839},
    {-58.2, 1794},
    {-44.1,1759},
    {-29.6, 1678},
    {-14.8, 1599},
    {0, 1521},
    {14.8, 1461},
    {29.6,1339},
    {44.0,1306},
    {58.2,1260},
    {71.9,1175},
    {85.1,1071}

  });
  return (int) t.lookup(theta_per_meter.degrees());
}

int Car::steering_for_angle(Angle theta)
{
  static const LookupTable t(
  {
    {-30, 1929},
    {-25, 1839},
    {-20, 1794},
    {-15,1759},
    {-10, 1678},
    {-5, 1599},
    {0, 1521},
    {5, 1461},
    {10,1339},
    {15,1306},
    {20,1260},
    {25,1175},
    {30,1071}

  });
  return (int) t.lookup(theta.degrees());
}



Angle Car::angle_for_steering(int str) {
  static const LookupTable t(
  {
    {1071, 30},
    {1175, 25},
    {1260, 20},
    {1306, 15},
    {1339, 10},
    {1461, 5},
    {1521, 0},
    {1599, -5},
    {1678, -10},
    {1759, -15},
    {1794, -20},
    {1839, -25},
    {1929, -30}
  });

  return Angle::degrees(t.lookup(str));

}

#include <vector>
#include <array>
#include "lookup_table.h"

int Car::esc_for_velocity(double v)
{
  static const LookupTable t(
  {
    {-2., 1200},
    {-1., 1250},
    {-.1, 1326},
    {0.0,  1500},
    {0.1, 1620},
    {2.0, 1671},
    {3.3, 1700},
    {4.1, 1744},
    {5, 1770},
    {7, 1887},
    {9.5,1895},
    {20, 2000}
  });
  return t.lookup(v);

  /* old, ts table
  static const LookupTable t(
  {
    {-2., 1200},
    {-1., 1250},
    {0.0,  1500},
    {0.1, 1645},
    {0.34, 1659},
    {0.85, 1679},
    {1.2, 1699},
    {1.71, 1719},
    {1.88, 1739},
    {2.22, 1759},
    {2.6, 1779},
    {3.0, 1799},
    {14.0, 2000}
  });
  return t.lookup(v);
  */

}

void test_car() {

  Car car;
  for(auto i:linspace(-35,25,5)) {
     cout << "degrees: " << i << " steering: " << car.steering_for_angle(Angle::degrees(i)) << endl;
  }
}
