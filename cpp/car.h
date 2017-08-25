#ifndef CAR_H
#define CAR_H

#include <list>
#include <string>
#include <fstream>
#include "ackerman.h"
#include "geometry.h"
#include "dynamics.h"
#include "work_queue.h"
#include "usb.h"
#include "speedometer.h"
#include "async_buf.h"
#include "socket_server.h"

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
  Dynamics original_dynamics;
  int reading_count = 0;

  Speedometer front_right_wheel, front_left_wheel, back_left_wheel, back_right_wheel;
  Angle heading_adjustment;
  Angle zero_heading_angle;

  // calibrated measurements
  double meters_per_odometer_tick;
  double rear_meters_per_odometer_tick;
  double gyro_adjustment_factor;
  int center_steering_us;
  int min_forward_esc;
  int min_reverse_esc;
  int reverse_center_steering_us; // ?!
  double front_wheelbase_width;
  double rear_wheelbase_width;
  double wheelbase_length;

  // accessors
  void set_zero_heading();
  Angle get_zero_heading();
  Angle get_heading();

  const Speedometer & get_back_left_wheel() const {
    return back_left_wheel;
  }

  const Speedometer & get_back_right_wheel() const {
    return back_right_wheel;
  }

  const Speedometer & get_front_left_wheel() const {
    return front_left_wheel;
  }

  const Speedometer & get_front_right_wheel() const {
    return front_right_wheel;
  }

  /*
  int get_odometer_front_left() {
    return current_dynamics.odometer_front_left;
  }
  int get_odometer_front_right() {
    return front_right_wheel.last_ticks;
  }
  int get_odometer_back_left() {
    return current_dynamics.odometer_back_left;
  }
  int get_odometer_back_right() {

    return current_dynamics.odometer_back_right;
  }
*/
  int get_reading_count() {
    return reading_count;
  }

  int get_esc() {
    return current_dynamics.esc;
  }

  int get_str() {
    return current_dynamics.str;
  }

  double get_voltage(){
    return current_dynamics.battery_voltage;
  };

  int get_spur_odo() {
    return current_dynamics.spur_odo;
  }

  inline Point get_front_position(){
    return ackerman.front_position();
  };

  inline Point get_rear_position(){
    return ackerman.rear_position();
  };

  inline double get_velocity() {
      return (front_left_wheel.get_smooth_velocity() + front_right_wheel.get_smooth_velocity()) / 2;
  }

  double get_rear_velocity() {
    return (back_left_wheel.get_velocity() + back_right_wheel.get_velocity()) / 2;
  }

  double get_acceleration();
  double get_smooth_acceleration();

  double get_time_in_seconds();

  inline int get_usb_error_count() {
    return usb_error_count;
  }

  int steering_for_angle(Angle theta);
  int steering_for_curvature(Angle theta_per_meter);
  Angle angle_for_steering(int str);

  int esc_for_velocity(double v);

  // control
  void send_command(string command);
  void set_rc_mode();
  void set_manual_mode();
  void set_esc_and_str(unsigned esc, unsigned str);

  // logging
  void begin_recording_input(string path);
  void end_recording_input();

  void begin_recording_state(string path);
  void end_recording_state();
  void write_state();


  // misc state
  int commanded_esc;
  int commanded_str;



  // infrastructure
  bool process_line_from_log(string s);
  void apply_dynamics(Dynamics & d);
  Usb usb;

  string command_from_socket = "";
private:
  unique_ptr<async_buf> input_recording_buf;
  unique_ptr<ostream> input_recording_file;

  unique_ptr<async_buf> state_buf;
  unique_ptr<ostream> state_recording_file;

  void usb_thread_start();
  WorkQueue<string> usb_queue;
  thread usb_thread;
  void connect_usb();

  void read_configuration(string path);
  list<WorkQueue<Dynamics>*> listeners;
  std::mutex listeners_mutex;

  SocketServer socket_server;
  void process_socket();
};




void test_car();

#endif // CAR_H
