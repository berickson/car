#include "car_menu.h"
#include "menu.h"
#include "system.h"
#include "console_menu.h"
#include "fake_car.h"
#include "filenames.h"


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


string track_name = "desk";
string get_track_name() {
  return track_name;
}
void set_track_name(string s) {
  track_name = s;
}


CarMenu::CarMenu() {
}

void run_car_menu() {
  FakeCar car;

  SubMenu pi_menu {
    {"shutdown",shutdown},
    {"reboot",reboot},
    {"restart",restart}
  };


  SubMenu track_selection_menu{};
  vector<string> track_names = FileNames().get_track_names();
  selection_menu<string>(track_selection_menu, track_names, get_track_name, set_track_name);


  SubMenu route_menu {
    {"track:",&track_selection_menu}

  };

  SubMenu mid_menu {
    {"routes",&route_menu},
    {"pi",&pi_menu}
  };

  SubMenu car_menu {
    {[&car](){return get_first_ip_address();}, &mid_menu},
    {[&car](){return "v: " + to_string(car.get_voltage());}},
    {[&car](){return "front: " + to_string(car.get_front_position());}},
    {[&car](){return "usb errors: " + to_string(car .get_usb_error_count());}},
    {[&car](){return "heading: " + to_string(car.get_heading_degrees());}},
    {[&car](){return "rear: " + to_string(car.get_rear_position());}},
    {[&car](){return "odo_fl: " + to_string(car.get_odometer_front_left());}},
    {[&car](){return "odo_fr: " + to_string(car.get_odometer_front_right());}},
    {[&car](){return "odo_bl: " + to_string(car.get_odometer_back_left());}},
    {[&car](){return "odo_br: " + to_string(car.get_odometer_back_right());}}

  };

  ConsoleMenu menu(&car_menu);
  menu.run();
}

void test_car_menu() {
  run_car_menu();
}
