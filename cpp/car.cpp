#include "car.h"
#include "config.h"

#include <iostream>
#include "lookup_table.h"
#include "split.h"
#include "logger.h"
#include <chrono>
#include "async_buf.h"
#include "socket_server.h"
#include "json.hpp"
#include <pthread.h>

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;




Car::Car(bool online) {
  log_entry_exit("Car::Car");
  log_info("reading configuration");
  read_configuration(config_path);
  front_right_wheel.meters_per_tick = this->meters_per_odometer_tick;
  front_left_wheel.meters_per_tick = this->meters_per_odometer_tick;
  back_left_wheel.meters_per_tick = this->rear_meters_per_odometer_tick;
  back_right_wheel.meters_per_tick = this->rear_meters_per_odometer_tick;
  motor.meters_per_tick = this->motor_meters_per_odometer_tick;
  log_info("all wheels set");
  this->online = online;
  reset_odometry();
  if(online) {
    connect_usb();
    connect_lidar();
  }
}

Car::~Car() {
  quit=true;
  if(usb_thread.joinable())
    usb_thread.join();
  if(lidar_thread.joinable())
    lidar_thread.join();
}

void Car::connect_lidar() {
  lidar_thread = thread(&Car::lidar_thread_start, this);
  pthread_setname_np(lidar_thread.native_handle(), "car_lidar");
}

void Car::connect_usb() {
  usb_thread = thread(&Car::usb_thread_start, this);
  pthread_setname_np(usb_thread.native_handle(), "car_usb");
}

string Car::get_mode() {
  if(rc_mode_enabled ) 
    return "automatic";

  if(command_from_socket == "record")
    return "recording";

  return "manual";
}

void Car::socket_get_scan(vector<string> & params) {
    int scan_to_skip = -1;
    if(params.size() > 1) {
      try {
        scan_to_skip = atoi(params[1].c_str());
      } catch (...) {
        log_warning("invalid scan number requested");
      }
    }

    bool found = false;

    //lidar.get_scan();
    if(recent_scans.size()>0) {
      auto wait_end_time = system_clock::now() + milliseconds(200);
      while ( !found && system_clock::now()  < wait_end_time) {
        {
          lock_guard<mutex> lock(recent_scans_mutex);
          LidarScan & scan = recent_scans.front();
          if(scan.scan_number != scan_to_skip) {
            found = true;
            socket_server.send_response(recent_scans.front().get_json().dump());
            break;
          }
        }
        this_thread::sleep_for(chrono::milliseconds(1));
      }
    }
    if (!found) {
      LidarScan fake;
      socket_server.send_response(fake.get_json());
    }
}

void Car::process_socket() {
  while(true){
    string full_request = socket_server.get_request();
    if(full_request.length()==0) return;
    log_info("socket full request: \"" + full_request + "\"");
    auto params = split(full_request, ',');
    string request = params[0];
    //log_info("got socket request: \"" + request + "\"");
    if(request=="get_scan") {
      socket_get_scan(params);
    }
    else if(request=="get_state"){
      nlohmann::json j;
      
      
      stringstream reply;
      j["velocity"] = get_velocity();
      j["v_bat"] = get_voltage();
      // https://www.invensense.com/wp-content/uploads/2015/02/MPU-9150-Register-Map.pdf p.6
      j["temperature"] = get_temperature()/340.0 + 35.0; // constants from https://playground.arduino.cc/Main/MPU-9150
      //j["temperature"] = (get_temperature() + 12412.0)/340.0; // constants from https://playground.arduino.cc/Main/MPU-9150
      j["mode"] = get_mode();
      j["heading"] = get_heading().radians();
      j["bl"] = get_back_left_wheel().get_json_state();
      j["br"] = get_back_right_wheel().get_json_state();
      j["fl"] = get_front_left_wheel().get_json_state();
      j["fr"] = get_front_right_wheel().get_json_state();
      j["motor"]= get_motor().get_json_state();
      j["front_x"] = get_front_position().x;
      j["front_y"] = get_front_position().y;
      j["go_enabled"] = get_go_enabled();


      socket_server.send_response(j.dump());
    } else if(request=="go") {
      log_info("go");
      command_from_socket = "go";
      socket_server.send_response("ok");
    } else if(request=="stop") {
      command_from_socket = "stop";
      log_info("stop requested from socket");
      set_manual_mode();
      socket_server.send_response("ok");
    } else if(request=="record") {
      command_from_socket = ("record");
      log_info("record requested from socket");
      set_manual_mode();
      socket_server.send_response("ok");
    } else if (request=="reset_odometer") {
      log_info("reset_odometer requested from socket");
      reset_odometry();
      socket_server.send_response("ok");
    } else {
        socket_server.send_response("Error, unknown command: "+request);
    }
  }
}

void Car::lidar_thread_start() {
  log_entry_exit w("lidar thread");
  lidar.run();
  while(!quit) {
    try {
      bool got_scan = lidar.try_get_scan(1);
      if(got_scan) {
        int scan_number;
        {
          lock_guard<std::mutex> lock(recent_scans_mutex);
          recent_scans.emplace_front(lidar.current_scan);
          scan_number = lidar.current_scan.scan_number;
          while(recent_scans.size() > 10) {
            recent_scans.pop_back();
          }
        }
        log_info("got scan " + to_string(scan_number));
      }
    }
    catch (string error_string) {
      log_error("error caught in lidar_thread_start"+error_string);
    }
    catch (...) {
      log_error("error caught in lidar_thread_start");
    }
  }
  if(quit) {
    log_info("lidar_thread exiting because quit == true");
  }
}

void Car::usb_thread_start() {
  try {
    log_entry_exit w("usb thread");
    socket_server.open_socket(5571);
    usb.add_line_listener(&usb_queue);
    usb.write_on_connect("\ntd+\n");
    usb.run("/dev/ttyACM1");
    while(!quit) {
      try {
        StampedString line;
        if(usb_queue.try_pop(line,1)) {
          log_warning_if_duration_exceeded w("processing usb line", 10ms);
          process_line_from_log(line);
        }
        process_socket();
      }
      catch (string error_string) {
        log_error("error caught in usb loop"+error_string);
      }
      catch (...) {
        log_error("error caught in usb loop");
      }
    }
  }
  catch (string error_string) {
    log_error("error caught in usb_thread_start"+error_string);
  }
  catch (...) {
    log_error("error caught in usb_thread_start");
  }
}

// returns true if the line is a valid TD
bool Car::process_line_from_log(const StampedString & msg) {
  if(input_recording_file) {
    log_warning_if_duration_exceeded w("write input_recording_file",2ms);
    *input_recording_file << msg.to_string() << '\n'; //todo: make non-blocking
  }
  if(split(msg.message)[0]!="TD") {
    return false;
  }
  Dynamics d;
  bool ok = Dynamics::from_log_string(d,msg);
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
    log_warning((string) "bad dynamics: " + msg.to_string());
  }
  return ok;
}

void Car::send_command(string command) {
  usb.write_line(command);
}

void Car::set_rc_mode() {
  rc_mode_enabled = true;
  send_command("rc");

}

void Car::set_manual_mode() {
  rc_mode_enabled = false;
  send_command("m");
}

void Car::set_esc_and_str(unsigned esc, unsigned str)
{
  if(!rc_mode_enabled) {
    throw string("autonomous mode disabled");
  }
  send_command((string)"pse "+to_string(str)+","+to_string(esc));
  commanded_esc = esc;
  commanded_str = str;
}

void Car::beep()
{
  send_command((string)"beep");
}


void Car::begin_recording_state(string path) {
  end_recording_state();
  this->state_buf.reset(new async_buf(path));
  state_recording_file.reset(new ostream(state_buf.get()));

  *state_recording_file << Dynamics::csv_field_headers();
  *state_recording_file << ",v_smooth,a_smooth,v_fl_smooth,a_fl_smooth,v_fr_smooth,a_fr_smooth,v_bl_smooth,a_bl_smooth,v_br_smooth,a_br_smooth,commanded_esc,commanded_str" << endl;
}

void Car::write_state() {
  log_warning_if_duration_exceeded w("write_state", 10ms);
  if(state_recording_file) {
    *state_recording_file << current_dynamics.csv_fields() << ","
        << get_velocity() << "," << get_smooth_acceleration() << ","
        << front_left_wheel.get_smooth_velocity() << "," << front_left_wheel.get_smooth_acceleration() << ","
        << front_right_wheel.get_smooth_velocity() << "," << front_right_wheel.get_smooth_acceleration() << ","
        << back_left_wheel.get_smooth_velocity() << "," << back_left_wheel.get_smooth_acceleration() << ","
        << back_right_wheel.get_smooth_velocity() << "," << front_right_wheel.get_smooth_acceleration() << ","
        << commanded_esc << ","
        << commanded_str << '\n';
  }
}

void Car::end_recording_state() {
  state_recording_file.reset(nullptr);
  state_buf.reset(nullptr); // causes flush
}

void Car::begin_recording_input(string path) {
  end_recording_input();
  this->input_recording_buf.reset(new async_buf(path));
  input_recording_file.reset(new ostream(input_recording_buf.get()));
}

void Car::end_recording_input() {
  input_recording_file.reset(nullptr);
  input_recording_buf.reset(nullptr); // causes flush
}

void Car::apply_dynamics(Dynamics & d) {
  log_warning_if_duration_exceeded w("apply_dynamics", 10ms);
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
  if (d.control_mode == 'm' && previous.control_mode!='m') {
     rc_mode_enabled = false;
  }

  // correct heading with adjustment factor
  Angle d_theta = (d.yaw - previous.yaw);
  d_theta.standardize();
  //heading_adjustment += Angle::radians(d_theta.radians() * gyro_adjustment_factor);

  // if wheels have moved, update ackerman
  back_left_wheel.update_from_sensor(d.us, d.odo_bl_a, d.odo_bl_a_us, d.odo_bl_b, d.odo_bl_b_us);
  back_right_wheel.update_from_sensor(d.us, d.odo_br_a, d.odo_br_a_us, d.odo_br_b, d.odo_br_b_us);
  front_left_wheel.update_from_sensor(d.us, d.odo_fl_a, d.odo_fl_a_us, d.odo_fl_b, d.odo_fl_b_us);
  motor.update_from_sensor(d.us, d.spur_odo, d.spur_last_us);
  double wheel_distance_meters = front_right_wheel.update_from_sensor(d.us, d.odo_fr_a, d.odo_fr_a_us, d.odo_fr_b, d.odo_fr_b_us);

  if (reading_count > 2 && fabs(wheel_distance_meters) > 0.) { // adding 2 keeps out the big jump after a reset
    Angle outside_wheel_angle = angle_for_steering(previous.str);
    ackerman.move_right_wheel(outside_wheel_angle, wheel_distance_meters, get_heading().radians());
  }

  // tell lidar we have moved
  {
    Point p = get_front_position();
    lidar.set_pose(p.x, p.y, get_heading().radians());
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
  motor_meters_per_odometer_tick = config.get_double("motor_meters_per_odometer_tick");
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

void Car::reset_odometry(double start_offset) {
  //dynamics = Dynamics();
  original_dynamics = current_dynamics;

  set_zero_heading();

  ackerman = Ackerman(
    front_wheelbase_width,
    wheelbase_length,Point(0,start_offset),Angle::degrees(0));
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

double Car::get_acceleration()
{
  return current_dynamics.ax;
}

double Car::get_smooth_acceleration() {
  return (get_front_left_wheel().get_smooth_acceleration() + get_front_right_wheel().get_smooth_acceleration()) / 2.0;
}

double Car::get_time_in_seconds()
{
  return current_dynamics.ms * 1.0E-3 + (current_dynamics.us % 1000)* 1.0E-6;
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
        {-2., 1300},  // {-2., 1200},
        {-1., 1400},  // {-1., 1250},
        {-.1, 1450},  // {-.1, 1326},
        {0.0,  1500}, // {0.0,  1500},
        {0.1,  1550}, // {0.1,  1610},
        {0.5, 1560},  // {0.5, 1620},
        {2.0, 1570},  // {2.0, 1671},
        {3.3, 1630},  // {3.3, 1700},
        {4.1, 1680},  // {4.1, 1744},
        {5, 1710},    // {5, 1770},
        {7, 1827},    // {7, 1887},
        {9.5,1895},   // {9.5,1895},
        {20, 2000}    // {20, 2000}
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
