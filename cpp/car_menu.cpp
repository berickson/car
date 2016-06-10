#include "car_menu.h"
#include "menu.h"
#include "system.h"
#include "console_menu.h"




void shutdown(){
  system("sudo shutdown now");
}
void reboot(){
  system("sudo shutdown -r now");

}
void restart(){
  system("sudo service car restart");
}




SubMenu pi_menu {
  {"shutdown",shutdown},
  {"reboot",reboot},
  {"restart",restart}
};

SubMenu car_menu {
  {"pi",&pi_menu}
};



CarMenu::CarMenu() {
}

void run_car_menu() {

}

void test_car_menu() {
  ConsoleMenu menu(&car_menu);
  menu.run();
}
