#include "car.h"
#include "config.h"

#include <iostream>
using namespace std;

Car::Car(bool online) {
  read_configuration(config_path);
  this->online = online;
  reset_odometry();
}

void Car::read_configuration(string path){
  Config config;
  config.load_from_file(path);

  // odometery
  meters_per_odometer_tick = config.get_double("meters_per_odometer_tick");
  gyro_adjustment_factor = config.get_double("gyro_adjustment_factor");

  // esc and steering
  center_steering_us = config.get_double("center_steering_us");
  min_forward_esc = config.get_int("min_forward_esc");
  min_reverse_esc = config.get_int("min_reverse_esc");
  reverse_center_steering_us = config.get_int("reverse_center_steering_us");

  // car dimensions
  front_wheelbase_width_in_meters = config.get_double("front_wheelbase_width_in_meters");
  rear_wheelbase_width_in_meters = config.get_double("rear_wheelbase_width_in_meters");
  wheelbase_length_in_meters = config.get_double("wheelbase_length_in_meters");

  // infer general dimensions
  length = wheelbase_length_in_meters;
  width = front_wheelbase_width_in_meters; // ?! why front?
}

void Car::reset_odometry() {
  dynamics = Dynamics();

  velocity = 0.0;
  last_velocity = 0.0;
  heading_adjustment = 0.;
  odometer_start = 0;
  odometer_front_left_start = 0;
  odometer_front_right_start = 0;
  odometer_back_left_start = 0;
  odometer_back_right_start = 0;
  ackerman = Ackerman(
    front_wheelbase_width_in_meters,
    wheelbase_length_in_meters);
}

void test_car() {
  Car car;
  cout << "front_wheelbase_width_in_meters: " << car.front_wheelbase_width_in_meters << endl;
}
