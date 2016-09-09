#include "car.h"
#include "config.h"

#include <iostream>
#include "lookup_table.h"
#include "split.h"
#include "logger.h"

using namespace std;




Car::Car(bool online) {
  read_configuration(config_path);
  front_right_wheel.meters_per_tick = this->meters_per_odometer_tick;
  front_left_wheel.meters_per_tick = this->meters_per_odometer_tick;
  back_left_wheel.meters_per_tick = this->meters_per_odometer_tick;
  back_right_wheel.meters_per_tick = this->meters_per_odometer_tick;
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

void Car::process_line_from_log(string line) {
  if(input_recording_file.is_open()) {
    input_recording_file << line << endl; //todo: make non-blocking
  }
  if(split(line)[1]!="TD") {
    return;
  }
  Dynamics d;
  bool ok = Dynamics::from_log_string(d,line);
  if(ok) {
    apply_dynamics(d);
    for(auto listener:listeners) {
      listener->push(d);
    }
  }
  else {
    ++usb_error_count;
    cerr << "dynamics not ok for " << line << endl;
  }
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
}


void Car::begin_recording_input(string path) {
  end_recording_input();
  input_recording_file.open(path, ios_base::out);
}

void Car::end_recording_input() {
  if(input_recording_file.is_open())
    input_recording_file.flush();
    input_recording_file.close();
}

void Car::apply_dynamics(Dynamics & d) {
  // set all the dynamics variables
  reading_count++;

  Dynamics previous = current_dynamics;
  current_dynamics = d;
  if(reading_count == 1) {
    original_dynamics = d;
    return;
  }

  // update state
  Dynamics & current = current_dynamics;

  // correct heading with adjustment factor
  Angle d_theta = (current.yaw - previous.yaw);
  d_theta.standardize();
  heading_adjustment += Angle::radians(d_theta.radians() * gyro_adjustment_factor);

  // if wheels have moved, update ackerman
  back_left_wheel.update_from_sensor(current.us, current.odometer_back_left_last_us, current.odometer_back_left);
  back_right_wheel.update_from_sensor(current.us, current.odometer_back_right_last_us, current.odometer_back_right);
  front_left_wheel.update_from_sensor(current.us, current.odometer_front_left_last_us, current.odometer_front_left);
  double wheel_distance_meters = front_right_wheel.update_from_sensor(current.us, current.odometer_front_right_last_us, current.odometer_front_right);
  if (fabs(wheel_distance_meters) > 0.) {
    Angle outside_wheel_angle = angle_for_steering(previous.str);
    ackerman.move_right_wheel(outside_wheel_angle, wheel_distance_meters, get_heading().radians());
  }


  // update velocity
  front_right_wheel.update_from_sensor(current.us, current.odometer_front_right_last_us, current.odometer_front_right);


}

void Car::add_listener(WorkQueue<Dynamics>* listener) {
  listeners.push_back(listener);
}

void Car::remove_listener(WorkQueue<Dynamics>*listener) {
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

  heading_adjustment = Angle::degrees(0.);

  ackerman = Ackerman(
    front_wheelbase_width,
    wheelbase_length,-wheelbase_length,0,0);
}

Angle Car::get_heading_adjustment() {
  return heading_adjustment;
}

Angle Car::get_heading() {
  Angle a = (
        current_dynamics.yaw
        - original_dynamics.yaw
        + heading_adjustment);
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

}

void test_car() {

  Car car;
  for(auto i:linspace(-35,25,5)) {
     cout << "degrees: " << i << " steering: " << car.steering_for_angle(Angle::degrees(i)) << endl;
  }
}
