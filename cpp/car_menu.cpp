#include "car_menu.h"
#include "menu.h"
#include "system.h"
#include "console_menu.h"
#include "fake_car.h"
#include "filenames.h"
#include "string_utils.h"
#include <fstream>

#include <ncurses.h> // sudo apt-get install libncurses5-dev

struct {
  string track_name = "desk";
  string route_name = FileNames().get_route_names(track_name)[0];
  double max_a = 0.25;
  double max_v = 1.0;
  double t_ahead = 0.3;
  double d_ahead = 0.05;
  double k_smooth = 0.4;
  bool capture_video = false;
} car_settings;


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
  int rv = system("sudo service car restart");
  assert0(rv);
}


SubMenu route_selection_menu{};


string get_route_name() {
  return car_settings.route_name;
}

void set_route_name(string s) {
  car_settings.route_name = s;
}

void update_route_selection_menu() {
  route_selection_menu.items.clear();
  vector<string> route_names = FileNames().get_route_names(car_settings.track_name);
  selection_menu<string>(route_selection_menu, route_names, get_route_name, set_route_name);
}

string get_track_name() {
  return car_settings.track_name;
}

void set_track_name(string s) {
  car_settings.track_name = s;
  update_route_selection_menu();
}


// getters / setters for config
void set_max_a(double v){car_settings.max_a = v;}
double get_max_a(){return car_settings.max_a;}

void set_max_v(double v){car_settings.max_v = v;}
double get_max_v(){return car_settings.max_v;}

void set_t_ahead(double v){car_settings.t_ahead = v;}
double get_t_ahead(){return car_settings.t_ahead;}

void set_d_ahead(double v){car_settings.d_ahead = v;}
double get_d_ahead(){return car_settings.d_ahead;}

void set_k_smooth(double v){car_settings.k_smooth = v;}
double get_k_smooth(){return car_settings.k_smooth;}

void set_capture_video(double v){car_settings.capture_video = v;}
double get_capture_video(){return car_settings.capture_video;}



SubMenu pi_menu {
  {"shutdown",shutdown},
  {"reboot",reboot},
  {"restart",restart}
};

vector<double> linspace(double from, double to, double step) {
  vector<double> v;
  for(double d = from; d <= to; d+= step) {
    v.push_back(d);
  }
  return v;
}

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

  auto record = [&car]() {
    car.reset_odometry();
    FileNames f;
    string track_name = car_settings.track_name;
    string route_name = f.next_route_name(track_name);
    mkdir(f.get_route_folder(track_name,route_name));
    WorkQueue<string> listener;
    car.usb.add_line_listener(&listener);

    string recording_path = f.recording_file_path(track_name,route_name);
    fstream recording;
    recording.open(recording_path,ios_base::out);


    string line;
    clear();
    move(0,0);
    printw("Recording - press any key to stop");
    refresh();
    while(listener.try_pop(line,1000)) {
      if(split(line)[1]=="TD") {
        recording << line;
      }
      if(getch()>0)
        break;
    }
    recording.close();
    car.usb.remove_line_listener(&listener);
    // todo: update display
    //
    car_settings.route_name = route_name;
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
    {[](){return (string)"track ["+car_settings.track_name+"]";},&track_selection_menu},
    {[](){return (string)"route ["+car_settings.route_name+"]";},&route_selection_menu},
    MenuItem("record",record),
    {[](){return (string)"max_a ["+format(car_settings.max_a)+"]";},&acceleration_menu},
    {[](){return (string)"max_v ["+format(car_settings.max_v)+"]";},&velocity_menu},
    {[](){return (string)"k_smooth ["+format(car_settings.k_smooth)+"]";},&k_smooth_menu},
    {[](){return (string)"t_ahead ["+format(car_settings.t_ahead)+"]";},&t_ahead_menu},
    {[](){return (string)"d_ahead ["+format(car_settings.d_ahead)+"]";},&d_ahead_menu},
    {[](){return (string)"cap video ["+format(car_settings.capture_video)+"]";},&capture_video_menu}

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
    {[&car](){return "heading: " + format(car.get_heading_degrees());}},
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
