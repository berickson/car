#include "car_controller.h"
#include "menu.h"
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
#include "camera.h"
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


SubMenu route_selection_menu{};


string get_route_name() {
  return run_settings.route_name;
}

void set_route_name(string s) {
  run_settings.route_name = s;
}

void update_route_selection_menu() {
  route_selection_menu.items.clear();
  vector<string> route_names = FileNames().get_route_names(run_settings.track_name);
  string max_name = "";
  if(route_names.size() > 0) {
      max_name = *max_element(route_names.begin(),route_names.end()) ;
  };
  set_route_name(max_name);
  selection_menu<string>(route_selection_menu, route_names, get_route_name, set_route_name);
}

string get_track_name() {
  return run_settings.track_name;
}

void set_track_name(string s) {
  run_settings.track_name = s;
  update_route_selection_menu();
}


// getters / setters for config
void set_max_accel_lat(double v){run_settings.max_accel_lat = v;}
double get_max_accel_lat(){return run_settings.max_accel_lat;}

void set_max_accel(double v){run_settings.max_accel = v;}
double get_max_accel(){return run_settings.max_accel;}

void set_max_decel(double v){run_settings.max_decel = v;}
double get_max_decel(){return run_settings.max_decel;}

void set_max_v(double v){run_settings.max_v = v;}
double get_max_v(){return run_settings.max_v;}

void set_t_ahead(double v){run_settings.t_ahead = v;}
double get_t_ahead(){return run_settings.t_ahead;}

void set_d_ahead(double v){run_settings.d_ahead = v;}
double get_d_ahead(){return run_settings.d_ahead;}

void set_k_smooth(double v){run_settings.k_smooth = v;}
double get_k_smooth(){return run_settings.k_smooth;}

void set_k_p(double v){run_settings.steering_k_p = v;}
double get_k_p(){return run_settings.steering_k_p;}

void set_k_i(double v){run_settings.steering_k_i = v;}
double get_k_i(){return run_settings.steering_k_i;}

void set_k_d(double v){run_settings.steering_k_d = v;}
double get_k_d(){return run_settings.steering_k_d;}

void set_v_k_p(double v){run_settings.v_k_p = v;}
double get_v_k_p(){return run_settings.v_k_p;}

void set_v_k_i(double v){run_settings.v_k_i = v;}
double get_v_k_i(){return run_settings.v_k_i;}

void set_v_k_d(double v){run_settings.v_k_d = v;}
double get_v_k_d(){return run_settings.v_k_d;}

void set_prune_max(double v){run_settings.prune_max = v;}
double get_prune_max(){return run_settings.prune_max;}

void set_prune_tolerance(double v){run_settings.prune_tolerance = v;}
double get_prune_tolerance(){return run_settings.prune_tolerance;}


void set_slip_rate(double v){run_settings.slip_rate = v;}
double get_slip_rate(){return run_settings.slip_rate;}

void set_slip_slop(double v){run_settings.slip_slop = v;}
double get_slip_slop(){return run_settings.slip_slop;}


void set_capture_video(double v){run_settings.capture_video = v;}
double get_capture_video(){return run_settings.capture_video;}

void set_crash_recovery(double v){run_settings.crash_recovery = v;}
double get_crash_recovery(){return run_settings.crash_recovery;}

void set_optimize_velocity(double v){run_settings.optimize_velocity = v;}
double get_optimize_velocity(){return run_settings.optimize_velocity;}


SubMenu pi_menu {
  MenuItem{"shutdown",shutdown},
  MenuItem{"reboot",reboot},
  //{"restart",restart},
  //{"update software 2",update_software}
};


SubMenu max_accel_lat_menu{};
SubMenu max_accel_menu{};
SubMenu max_decel_menu{};
SubMenu max_v_menu{};

SubMenu k_p_menu{};
SubMenu k_i_menu{};
SubMenu k_d_menu{};

SubMenu v_k_p_menu{};
SubMenu v_k_i_menu{};
SubMenu v_k_d_menu{};

SubMenu prune_max_menu{};
SubMenu prune_tolerance_menu{};

SubMenu k_smooth_menu{};
SubMenu t_ahead_menu{};
SubMenu d_ahead_menu{};
SubMenu capture_video_menu{};
SubMenu crash_recovery_menu{};
SubMenu optimize_velocity_menu{};

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
  update_route_selection_menu();
  run_settings.write_to_file_json(run_settings_path);
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

