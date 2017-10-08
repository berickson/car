#include "car_controller.h"
#include "route.h"
#include "system.h"
#include "fake_car.h"
#include "file_names.h"
#include "string_utils.h"
#include <fstream>
#include <sstream>
#include <bitset>
#include "car_ui.h"
#include "driver.h"
#include "run_settings.h"
#include <unistd.h> // usleep
#include "stereo_camera.h"
#include "logger.h"


string run_settings_path = "run_settings.json";

RunSettings run_settings;

void assert0(int n) {
  if(n!=0) {
    throw (string) "expected zero, got" + to_string(n);
  }

}


void shutdown(){
  int rv =  system("sudo shutdown now");
  assert0(rv);
}
void reboot(){
  int rv = system("sudo shutdown -r now");
  assert0(rv);

}
void restart(){
  log_info("restarting service based on user request");
  int rv = system("sudo ./run");
  terminate();
  assert0(rv);
}

void update_software() {
  log_info("restarting based on user update request");
  int ignored __attribute__((unused));
  ignored = system("git pull");
  ignored = system("./build");
  ignored = system("sudo ./run");
  log_info("terminating");
  terminate();
}

void go(Car& car) {
  log_entry_exit w("go");
  try {
    run_settings.load_from_file_json(run_settings_path);
    auto f = FileNames();
    string & track_name = run_settings.track_name;
    string & route_name = run_settings.route_name;
    string run_name = f.next_run_name(track_name, route_name);
    string run_folder = f.get_run_folder(track_name,route_name, run_name);
    string runs_folder = f.get_runs_folder(track_name,route_name);
    mkdir(runs_folder);
    mkdir(run_folder);
    run_settings.write_to_file_json(f.config_file_path(track_name, route_name, run_name));
    run_settings.write_to_file_json(run_settings_path);
    car.reset_odometry();
    string input_path = f.path_file_path(track_name,route_name);
    Route rte;
    log_info("loading route: " + input_path);
    rte.load_from_file(input_path);
    log_info("route loaded");
    StereoCamera camera;
    if(run_settings.capture_video) {
      log_info("capture video enabled, warming up camera");
      camera.warm_up();
    } else {
      log_info("capturing video not enabled");
    }


    log_info("preparing route");
    rte.smooth(run_settings.k_smooth);
    rte.prune(run_settings.prune_max, run_settings.prune_tolerance);
    if(run_settings.optimize_velocity) {
      rte.optimize_velocity(run_settings.max_v, run_settings.max_accel_lat, run_settings.max_accel, run_settings.max_decel);
    } else {
      log_info("using saved velocities");
    }
    if(run_settings.capture_video) {
      vector<string> video_paths = f.stereo_video_file_paths(track_name,route_name,run_name);
      camera.begin_recording(video_paths[0],video_paths[1]);
    }

    string recording_file_path = f.recording_file_path(track_name, route_name, run_name);
    car.begin_recording_input(recording_file_path);
    car.begin_recording_state(f.state_log_path(track_name, route_name, run_name));

    string state_file_path = f.state_log_path(track_name, route_name, run_name);
    car.begin_recording_state(state_file_path);
    std::string error_text = "";
    try {
      Driver d(car,run_settings);
      d.drive_route(rte);
      log_info("back from drive_route");
    } catch (std::string e) {
      error_text = e;
      log_error(e);
    }
    if(run_settings.capture_video) {
      camera.end_recording();
    }
    car.end_recording_input();
    car.end_recording_state();
    log_info("making path");

    string path_file_path = f.path_file_path(track_name, route_name, run_name);
    write_path_from_recording_file(recording_file_path, path_file_path);
    log_info("recording: " + recording_file_path);
    log_info("track: " + track_name);
    log_info("route: " + route_name);
    log_info("run: " + run_name);
    string dynamics_path = f.dynamics_file_path(track_name,route_name,run_name);
    log_info(dynamics_path);
    log_info(recording_file_path);
    write_dynamics_csv_from_recording_file(recording_file_path, dynamics_path);
    rte.write_to_file(f.planned_path_file_path(track_name, route_name, run_name));
    run_settings.write_to_file_json(run_settings_path);

    if(error_text.size()) {
      log_error((string) "Error during play route: "+error_text);
    }
  } catch (string s) {
    log_error("go caught error:" + s);
  } catch (...) {
    log_error("unknown error caught in go");
  }
}

void record(Car& car) {
  log_entry_exit w("record");
  run_settings.load_from_file_json(run_settings_path);  
  car.reset_odometry();
  FileNames f;
  string track_name = run_settings.track_name;
  string route_name = f.next_route_name(track_name);
  log_info((string)"recording route name: " + route_name);
  mkdir(f.get_routes_folder(track_name));
  mkdir(f.get_route_folder(track_name,route_name));

  StereoCamera camera;
  if(run_settings.capture_video) {
    camera.warm_up();
    vector<string> video_paths = f.stereo_video_file_paths(track_name,route_name);
    camera.begin_recording(video_paths[0],video_paths[1]);
  }

  string recording_path = f.recording_file_path(track_name,route_name);
  car.begin_recording_input(recording_path);
  car.begin_recording_state(f.state_log_path(track_name,route_name));

  while(car.command_from_socket == "record") {
    usleep(30000);
  }

  car.end_recording_input();
  car.end_recording_state();

  if(run_settings.capture_video){
    camera.end_recording();
  }

  log_info("done recording - making path");

  string path_file_path = f.path_file_path(track_name, route_name);
  write_path_from_recording_file(recording_path, path_file_path);
  write_dynamics_csv_from_recording_file(recording_path, f.dynamics_file_path(track_name, route_name));

  run_settings.route_name = route_name;
  return;
}



void run_car_socket() {
  log_entry_exit w("run_car_socket");
  try {
    Car car;
    run_settings.load_from_file_json(run_settings_path);
    
    while(true) {
     
      if(car.command_from_socket == "go") {
        try {
           go(car);
        } catch (...) {
           log_error("exception caught in run_car_socket go");
        }
      }
      if(car.command_from_socket == "record") {
        run_settings.track_name = "desk";
        try {
           record(car);
        } catch (...) {
           log_error("exception caught in run_car_socket record");
        }
      }
      car.command_from_socket = "";
      usleep(30000);
    } 
  }
  catch (...) {
    log_error("exception caught in run_car_socket");
  }
}

