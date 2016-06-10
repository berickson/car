#include "car_menu.h"
#include "menu.h"
#include "system.h"
#include "console_menu.h"
#include "fake_car.h"


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




CarMenu::CarMenu() {
}

void run_car_menu() {
  FakeCar car;

  SubMenu pi_menu {
    {"shutdown",shutdown},
    {"reboot",reboot},
    {"restart",restart}
  };

  SubMenu car_menu {
    {[&car](){return "v: " + to_string(car.get_voltage());}},
    {[&car](){return "front: " + to_string(car.get_front_position());}},
    {[&car](){return "heading: " + to_string(car.get_heading_degrees());}},
    {[&car](){return "rear: " + to_string(car .get_rear_position());}},
    {[&car](){return "usb errors: " + to_string(car .get_usb_error_count());}},
    {"pi",&pi_menu}
  };

  ConsoleMenu menu(&car_menu);
  menu.run();
}

void test_car_menu() {
  run_car_menu();
}
