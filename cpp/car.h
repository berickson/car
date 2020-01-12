#ifndef CAR_H
#define CAR_H

#include <list>
#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include "ackerman.h"
#include "geometry.h"
#include "dynamics.h"
#include "work_queue.h"
#include "usb.h"
#include "speedometer.h"
#include "async_buf.h"
#include "socket_server.h"
#include "lidar.h"
#include "../teensy/CarMessages.h"
#include "web_server.h"
#include "stereo_camera.h"


using namespace std;
using namespace std::chrono;

class Car
{
public:
  Car(bool online = true);
  ~Car();

  void reset_odometry(double start_offset = 0.0);

  void add_listener(WorkQueue<Dynamics2>*);
  void remove_listener(WorkQueue<Dynamics2>*);

#if defined(orange_car)
  string config_path = "car.ini";
#elif defined(blue_car)
  string config_path = "blue-car.ini";
#else
#error "could not find defined car"
#endif



  bool online = false;
  bool quit = false;
  unsigned long usb_error_count = 0;

  Ackerman ackerman;

  // state variables
  Dynamics2 current_dynamics;
  Dynamics2 original_dynamics;
  int reading_count = 0;
  system_clock::time_point last_scan_request_time;

  Speedometer front_right_wheel, front_left_wheel, motor;
  Angle heading_adjustment;
  Angle zero_heading_angle;

  // calibrated measurements
  double meters_per_odometer_tick;
  double rear_meters_per_odometer_tick;
  double motor_meters_per_odometer_tick;
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
  Angle get_zero_heading() const;
  Angle get_heading() const;



  const Speedometer & get_front_left_wheel() const {
    return front_left_wheel;
  }

  const Speedometer & get_front_right_wheel() const {
    return front_right_wheel;
  }

  const Speedometer & get_motor() const {
    return motor;
  }

  int get_reading_count() {
    return reading_count;
  }

  string get_mode();

  LidarScan get_latest_scan() const;
  string get_scan_json(int since);


  double get_voltage(){
    return current_dynamics.v_bat;
  };

  double get_temperature() {
    return current_dynamics.mpu_deg_f;
  }

  int get_spur_odo() {
    return current_dynamics.spur_odo;
  }

  inline Point get_front_position(){
    return ackerman.front_position();
  }

  inline Point get_rear_position() const {
    return ackerman.rear_position();
  }

  inline bool get_go_enabled() {
    return current_dynamics.go;
  }

  inline double get_velocity() const {
      return (front_left_wheel.get_velocity() + front_right_wheel.get_velocity()) / 2;
  }

  double get_rear_velocity() {
    return motor.get_velocity();
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
  void beep();
  
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
  bool process_line_from_log(const StampedString & s);
  void apply_dynamics(Dynamics2 & d);
  Usb usb;

  // lidar
  LidarUnit lidar;

  // camera
  StereoCamera camera;


  string command_from_socket = "";
private:
  bool rc_mode_enabled = false;
  unique_ptr<async_buf> input_recording_buf;
  unique_ptr<ostream> input_recording_file;

  unique_ptr<async_buf> state_buf;
  unique_ptr<ostream> state_recording_file;

  void lidar_thread_start();
  thread lidar_thread;
  std::list<LidarScan> recent_scans;
  mutable std::mutex recent_scans_mutex;
  void connect_lidar();

  void usb_thread_start();
  WorkQueue<StampedString> usb_queue;
  thread usb_thread;
  void connect_usb();

  void socket_thread_start();
  thread socket_thread;
  void start_socket();

  void web_server_thread_start();
  thread web_server_thread;
  void get_scan_handler(const Request &, Response & response);
  void get_state_handler(const Request &, Response & response);
  void video_handler(const Request &, Response & response);
  void start_web_server();

  void read_configuration(string path);
  ObservableTopic<Dynamics2> dynamics2_topic;
  //list<WorkQueue<Dynamics2>*> listeners;
  //std::mutex listeners_mutex;

  SocketServer socket_server;
  void process_socket();
  void socket_get_scan(vector<string> & params);
};




void test_car();

#endif // CAR_H
