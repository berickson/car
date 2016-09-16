#include "car_menu.h"
#include "menu.h"
#include "route.h"
#include "system.h"
#include "console_menu.h"
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

#include <ncurses.h> // sudo apt-get install libncurses5-dev



RunSettings run_settings;


string wheel_display_string(const Speedometer & wheel){
  return format(wheel.get_meters_travelled(),7,2)+"m "
         + format(wheel.get_velocity(),4,1)+"m/s "
         + format(wheel.get_ticks()) + "t";
}

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
void set_max_a(double v){run_settings.max_a = v;}
double get_max_a(){return run_settings.max_a;}

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
  {"shutdown",shutdown},
  {"reboot",reboot},
  //{"restart",restart},
  //{"update software 2",update_software}
};


SubMenu acceleration_menu{};
SubMenu velocity_menu{};

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


CarMenu::CarMenu() {
}

void go(Car& car, CarUI & ui) {
  log_info("entering go menu command");
  try {
    auto f = FileNames();
    string & track_name = run_settings.track_name;
    string & route_name = run_settings.route_name;
    string run_name = f.next_run_name(track_name, route_name);
    string run_folder = f.get_run_folder(track_name,route_name, run_name);
    string runs_folder = f.get_runs_folder(track_name,route_name);
    mkdir(runs_folder);
    mkdir(run_folder);
    run_settings.write_to_file(f.config_file_path(track_name, route_name, run_name));
    car.reset_odometry();
    string input_path = f.path_file_path(track_name,route_name);
    Route rte;
    ui.clear();
    ui.print("loading route\n");
    ui.refresh();
    rte.load_from_file(input_path);
    StereoCamera camera;
    if(run_settings.capture_video) {
      camera.warm_up();
    }


    ui.clear();
    ui.print("smoothing route\n");
    ui.refresh();
    rte.smooth(run_settings.k_smooth);
    ui.print("pruning route\n");
    rte.prune(run_settings.prune_max, run_settings.prune_tolerance);
    ui.refresh();

    ui.clear();
    ui.move(0,0);
    ui.refresh();
    if(run_settings.optimize_velocity) {
      rte.optimize_velocity(run_settings.max_v, run_settings.max_a);
      ui.print("optimized velocity\n");
    } else {
      ui.print("using saved velocities\n");
    }
    ui.print((string)"max_v calculated at " + format(rte.get_max_velocity(),4,1) + "\n\n");
    ui.print("[back] [] []  [go]");
    ui.refresh();
    if(ui.wait_key()!='4') {
      return;
    }

    ui.clear();
    ui.print((string)"playing route with max velocity " + format(rte.get_max_velocity()));
    ui.refresh();

    if(run_settings.capture_video) {
      vector<string> video_paths = f.stereo_video_file_paths(track_name,route_name,run_name);
      camera.begin_recording(video_paths[0],video_paths[1]);
    }

    string recording_file_path = f.recording_file_path(track_name, route_name, run_name);
    car.begin_recording_input(recording_file_path);
    std::string error_text = "";
    try {
      Driver d(car,ui,run_settings);
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
    ui.clear();
    ui.print("making path");
    log_info("making path");

    string path_file_path = f.path_file_path(track_name, route_name, run_name);
    write_path_from_recording_file(recording_file_path, path_file_path);
    rte.write_to_file(f.planned_path_file_path(track_name, route_name, run_name));

    if(error_text.size()) {
      ui.clear();
      ui.print((string) "Error:: \n"+error_text);
      log_error((string) "Error during play route: "+error_text);

      ui.print("[ok]");
      ui.refresh();
      ui.wait_key();
    } else {
      ui.clear();
      ui.print("Playback Success [ok]");
      ui.refresh();
      ui.wait_key();
    }

  } catch (string s) {
    ui.clear();
    ui.move(0,0);
    ui.print("error: " + s);
    log_error("go caught error:" + s);
    ui.refresh();
    ui.wait_key();

  }
  log_info("exiting go menu command");
}

void record(Car& car, CarUI & ui) {
  car.reset_odometry();
  FileNames f;
  string track_name = run_settings.track_name;
  string route_name = f.next_route_name(track_name);
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

  ui.clear();
  ui.move(0,0);
  ui.print("Recording - [stop]");
  ui.refresh();
  ui.wait_key();

  car.end_recording_input();

  if(run_settings.capture_video){
    camera.end_recording();
  }

  ui.clear();
  ui.print("making path");
  ui.refresh();

  string path_file_path = f.path_file_path(track_name, route_name);
  write_path_from_recording_file(recording_path, path_file_path);

  run_settings.route_name = route_name;
  update_route_selection_menu();
  return;
}


string calibration_string(int a) {
  std::bitset<8> x(a);
  stringstream ss;
   ss << x;
  return ss.str();
}

void run_car_menu() {
#ifdef RASPBERRY_PI
  Car car;
#else
  FakeCar car;
#endif
  CarUI ui;

  selection_menu<double>(acceleration_menu, linspace(0.25,10,0.25), get_max_a, set_max_a );
  selection_menu<double>(velocity_menu, linspace(0.5,20,0.5), get_max_v, set_max_v );
  selection_menu<double>(k_p_menu, linspace(0.,300,10), get_k_p, set_k_p );
  selection_menu<double>(k_i_menu, linspace(0.,30,0.5), get_k_i, set_k_i );
  selection_menu<double>(k_d_menu, linspace(0.,300,5), get_k_d, set_k_d );

  selection_menu<double>(v_k_p_menu, linspace(0.,3,0.25), get_v_k_p, set_v_k_p );
  selection_menu<double>(v_k_i_menu, linspace(0.,3,0.25), get_v_k_i, set_v_k_i );
  selection_menu<double>(v_k_d_menu, linspace(0.,3,0.25), get_v_k_d, set_v_k_d );

  selection_menu<double>(prune_max_menu, linspace(0.0 ,3.0, 0.1), get_prune_max, set_prune_max );
  selection_menu<double>(prune_tolerance_menu, linspace(0.0, 0.2, 0.01), get_prune_tolerance, set_prune_tolerance);


  selection_menu<double>(k_smooth_menu, linspace(0.,1,0.1), get_k_smooth, set_k_smooth );
  selection_menu<double>(t_ahead_menu, linspace(0.,1,0.1), get_t_ahead, set_t_ahead );
  selection_menu<double>(d_ahead_menu, linspace(0.,.1,0.01), get_d_ahead, set_d_ahead );
  selection_menu<double>(capture_video_menu, {0,1}, get_capture_video, set_capture_video );
  selection_menu<double>(crash_recovery_menu, {0,1}, get_crash_recovery, set_crash_recovery );
  selection_menu<double>(optimize_velocity_menu, {0,1}, get_optimize_velocity, set_optimize_velocity );


  SubMenu track_selection_menu{};
  vector<string> track_names = FileNames().get_track_names();
  selection_menu<string>(track_selection_menu, track_names, get_track_name, set_track_name);

  update_route_selection_menu();

  SubMenu route_menu {
    {[](){return (string)"track ["+run_settings.track_name+"]";},&track_selection_menu},
    {[](){return (string)"route ["+run_settings.route_name+"]";},&route_selection_menu},
    MenuItem("go...",[&car,&ui](){go(car,ui);}),
    {[&car](){
        return (string) calibration_string(car.current_dynamics.calibration_status)
            + " " + format(car.get_heading().degrees(),5,1) + "° "
            + format(car.get_reading_count(),6,0)
            + " " + format(car.current_dynamics.battery_voltage,4,1)+"v"; }},
    MenuItem("record",[&car,&ui](){record(car,ui);}),
    {[](){return (string)"max_a ["+format(run_settings.max_a)+"]";},&acceleration_menu},
    {[](){return (string)"max_v ["+format(run_settings.max_v)+"]";},&velocity_menu},

    {[](){return (string)"k_p ["+format(run_settings.steering_k_p)+"]";},&k_p_menu},
    {[](){return (string)"k_i ["+format(run_settings.steering_k_i)+"]";},&k_i_menu},
    {[](){return (string)"k_d ["+format(run_settings.steering_k_d)+"]";},&k_d_menu},

    {[](){return (string)"v_k_p ["+format(run_settings.v_k_p)+"]";},&v_k_p_menu},
    {[](){return (string)"v_k_i ["+format(run_settings.v_k_i)+"]";},&v_k_i_menu},
    {[](){return (string)"v_k_d ["+format(run_settings.v_k_d)+"]";},&v_k_d_menu},


    {[](){return (string)"prune tol ["+format(run_settings.prune_tolerance)+"]";},&prune_tolerance_menu},
    {[](){return (string)"prune max ["+format(run_settings.prune_max)+"]";},&prune_max_menu},

    {[](){return (string)"k_smooth ["+format(run_settings.k_smooth)+"]";},&k_smooth_menu},
    {[](){return (string)"t_ahead ["+format(run_settings.t_ahead)+"]";},&t_ahead_menu},
    {[](){return (string)"d_ahead ["+format(run_settings.d_ahead)+"]";},&d_ahead_menu},
    {[](){return (string)"cap video ["+format(run_settings.capture_video)+"]";},&capture_video_menu}

  };

  SubMenu mid_menu {
    {"routes",&route_menu},
    {"pi",&pi_menu}
  };


  SubMenu car_menu {
    {[&car](){return get_first_ip_address();}, &mid_menu},
    {[&car](){return "v: " + format(car.get_voltage());}},
    {[&car](){return "front: " + to_string(car.get_front_position());}},
    {[&car](){return "usb readings: " + format(car.get_reading_count());}},
    {[&car](){return "usb errors: " + format(car .get_usb_error_count());}},
    {[&car](){return "reset odo ";}, [&car]() {car.reset_odometry();}},
    {[&car](){return "heading: " + format(car.get_heading().degrees());}},
    {[&car](){return "heading_adj: " + format(car.get_heading_adjustment().degrees());}},
    {[&car](){return "rear: " + to_string(car.get_rear_position());}},
    {[&car](){return "fl: " + wheel_display_string(car.get_front_left_wheel());}},
    {[&car](){return "fr: " + wheel_display_string(car.get_front_right_wheel());}},
    {[&car](){return "bl: " + wheel_display_string(car.get_back_left_wheel());}},
    {[&car](){return "br: " + wheel_display_string(car.get_back_right_wheel());}},
    {[&car](){return "str: " + format(car.get_str());}},
    {[&car](){return "esc: " + format(car.get_esc());}}

  };

  ConsoleMenu menu(&car_menu);
  menu.run();
}

void test_car_menu() {
  run_car_menu();
}
