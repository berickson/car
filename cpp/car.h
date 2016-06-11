#ifndef CAR_H
#define CAR_H

#include <list>
#include <string>
#include "ackerman.h"
#include "geometry.h"
#include "dynamics.h"
#include "work_queue.h"
#include "usb.h"

using namespace std;

class Car
{
public:
  Car(bool online = true);
  ~Car();
  void reset_odometry();

  void add_listener(WorkQueue<Dynamics>*);
  void remove_listener(WorkQueue<Dynamics>*);

  string config_path = "/home/brian/car/python/car.ini";


  bool online = false;
  bool quit = false;
  bool usb_error_count = 0;

  Ackerman ackerman;

  // state variables
  Dynamics current_dynamics;
  Dynamics previous_dynamics;
  Dynamics original_dynamics;
  int reading_count = 0;

  double velocity;
  double last_verified_velocity;
  double last_velocity;
  Angle heading_adjustment;

  int odometer_front_left_start;
  int odometer_front_right_start;
  int odometer_back_left_start;
  int odometer_back_right_start;

  // calibrated measurements
  double meters_per_odometer_tick;
  double gyro_adjustment_factor;
  int center_steering_us;
  int min_forward_esc;
  int min_reverse_esc;
  int reverse_center_steering_us; // ?!
  double front_wheelbase_width_in_meters;
  double rear_wheelbase_width_in_meters;
  double wheelbase_length_in_meters;

  double length;
  double width;

  // accessors
  Angle get_heading();
  double get_heading_degrees();
  double get_heading_radians();

  int get_odometer_front_left() {
    return current_dynamics.odometer_front_left;
  }
  int get_odometer_front_right() {
    return current_dynamics.odometer_front_right;
  }
  int get_odometer_back_left() {
    return current_dynamics.odometer_back_left;
  }
  int get_odometer_back_right() {
    return current_dynamics.odometer_back_right;
  }
  int get_reading_count() {
    return reading_count;
  }

  double get_voltage(){
    return current_dynamics.battery_voltage;
  };

  inline Point get_front_position(){
    return ackerman.front_position();
  };

  inline Point get_rear_position(){
    return ackerman.rear_position();
  };

  inline int get_usb_error_count() {
    return usb_error_count;
  }

  double angle_for_steering(int str);

  // infrastructure
  void process_line_from_log(string s);
  void apply_dynamics(Dynamics & d);
private:
  Usb usb;
  void usb_thread_start();
  WorkQueue<string> usb_queue;
  thread usb_thread;
  void connect_usb();

  void read_configuration(string path);
  list<WorkQueue<Dynamics>*> listeners;
};


void test_car();

#endif // CAR_H
