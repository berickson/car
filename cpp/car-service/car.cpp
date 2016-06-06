#include "car.h"
#include "config.h"

#include <iostream>
using namespace std;

Car::Car(bool online) {
  read_configuration(config_path);
  this->online = online;
  reset_odometry();
  if(online) {
    connect_usb();
  }
}

Car::~Car() {
  quit=true;
  usb_thread.join();
}

void Car::connect_usb() {
  usb_thread = thread(&Car::usb_thread_start, this);
}

void Car::usb_thread_start() {
  usb.add_line_listener(&usb_queue);
  usb.run();
  usb.write_line("td+"); // twice on purpose
  usb.write_line("td+");
  while(!quit) {
    string line;
    if(usb_queue.try_pop(line,1)) {
      Dynamics d;
      bool ok = Dynamics::from_log_string(d,line);
      if(ok) {
        for(auto listener:listeners) {
          listener->push(std::move(d));
        }
      }
      else {
        cout << "dynamics not ok for " << line << endl;
      }

    }
  }

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

  WorkQueue<Dynamics> listener;
  car.add_listener(&listener);
  for(int i=0;i<10;i++) {
    Dynamics d;
    if(listener.try_pop(d,100)) {
      cout << d.to_string() << endl;
    } else {
      cout << "timed out waiting for dynamics" << endl;
    }
  }
  car.remove_listener(&listener);
}
