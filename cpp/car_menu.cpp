#include "car_menu.h"
#include "menu.h"
#include "route.h"
#include "system.h"
#include "console_menu.h"
#include "fake_car.h"
#include "file_names.h"
#include "string_utils.h"
#include <fstream>
#include "car_ui.h"
#include "driver.h"
#include "run_settings.h"
#include <unistd.h> // usleep

#include <ncurses.h> // sudo apt-get install libncurses5-dev



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
  int rv = system("sudo ./run");
  assert0(rv);
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

void set_capture_video(double v){run_settings.capture_video = v;}
double get_capture_video(){return run_settings.capture_video;}



SubMenu pi_menu {
  {"shutdown",shutdown},
  {"reboot",reboot},
  {"restart",restart}
};


SubMenu acceleration_menu{};
SubMenu velocity_menu{};
SubMenu k_smooth_menu{};
SubMenu t_ahead_menu{};
SubMenu d_ahead_menu{};
SubMenu capture_video_menu{};



CarMenu::CarMenu() {
}






void run_car_menu() {
#define RASPBERRY_PI
#ifdef RASPBERRY_PI
  Car car;
#else
  FakeCar car;
#endif
  CarUI io;



  auto go = [&car,&io]() {
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
    io.clear();
    io.print("loading route\n");
    io.refresh();
    rte.load_from_file(input_path);

    io.clear();
    io.print("smoothing route\n");
    io.refresh();
    rte.smooth(run_settings.k_smooth);
    io.clear();
    io.move(0,0);
    io.print((string) "optimizing velocity for "+input_path+"\n");
    io.refresh();
    rte.optimize_velocity(run_settings.max_v, run_settings.max_a);
    io.print((string)"max velocity calculated at " + format(rte.get_max_velocity()) + "\n");
    io.print("done - press any key to play route");
    io.refresh();
    io.wait_key();

    io.clear();
    io.print((string)"playing route with max velocity " + format(rte.get_max_velocity()));
    io.refresh();
    string recording_file_path = f.recording_file_path(track_name, route_name, run_name);
    car.begin_recording_input(recording_file_path);
    Driver d(car,io,run_settings);
    d.drive_route(rte);
    car.end_recording_input();

    io.clear();
    io.print("making path");

    string path_file_path = f.path_file_path(track_name, route_name, run_name);
    write_path_from_recording_file(recording_file_path, path_file_path);
    } catch (string s) {
      io.clear();
      io.move(0,0);
      io.print("error: " + s);
      io.refresh();
      while(io.getkey()==-1);

    }
  };

  auto record = [&car,&io]() {
    car.reset_odometry();
    FileNames f;
    string track_name = run_settings.track_name;
    string route_name = f.next_route_name(track_name);
    mkdir(f.get_route_folder(track_name,route_name));

    string recording_path = f.recording_file_path(track_name,route_name);
    car.begin_recording_input(recording_path);

    string line;
    io.clear();
    io.move(0,0);
    io.print("Recording - press any key to stop");
    io.refresh();
    io.wait_key();

    car.end_recording_input();

    io.clear();
    io.print("making path");
    io.refresh();

    string path_file_path = f.path_file_path(track_name, route_name);
    write_path_from_recording_file(recording_path, path_file_path);


    // todo: update display
    //
    run_settings.route_name = route_name;
    update_route_selection_menu();
    return;
  };

  selection_menu<double>(acceleration_menu, linspace(0.25,10,0.25), get_max_a, set_max_a );
  selection_menu<double>(velocity_menu, linspace(0.5,20,0.5), get_max_v, set_max_v );
  selection_menu<double>(k_smooth_menu, linspace(0.,1,0.1), get_k_smooth, set_k_smooth );
  selection_menu<double>(t_ahead_menu, linspace(0.,1,0.1), get_t_ahead, set_t_ahead );
  selection_menu<double>(d_ahead_menu, linspace(0.,.1,0.01), get_d_ahead, set_d_ahead );
  selection_menu<double>(capture_video_menu, {0,1}, get_capture_video, set_capture_video );


  SubMenu track_selection_menu{};
  vector<string> track_names = FileNames().get_track_names();
  selection_menu<string>(track_selection_menu, track_names, get_track_name, set_track_name);

  update_route_selection_menu();

  SubMenu route_menu {
    {[](){return (string)"track ["+run_settings.track_name+"]";},&track_selection_menu},
    {[](){return (string)"route ["+run_settings.route_name+"]";},&route_selection_menu},
    MenuItem("go...",go),
    MenuItem("record",record),
    {[](){return (string)"max_a ["+format(run_settings.max_a)+"]";},&acceleration_menu},
    {[](){return (string)"max_v ["+format(run_settings.max_v)+"]";},&velocity_menu},
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
    {[&car](){return "odo_fl: " + format(car.get_odometer_front_left());}},
    {[&car](){return "odo_fr: " + format(car.get_odometer_front_right());}},
    {[&car](){return "odo_bl: " + format(car.get_odometer_back_left());}},
    {[&car](){return "odo_br: " + format(car.get_odometer_back_right());}}

  };

  ConsoleMenu menu(&car_menu);
  menu.run();
}

void test_car_menu() {
  run_car_menu();
}
