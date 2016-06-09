#include "car_menu.h"
#include "menu.h"
#include "system.h"


CarMenu::CarMenu() {

}

void test_car_menu() {
  int x = 3;

  SubMenu m2 = {

    {[&]()->string {return "ip: " + get_first_ip_address();}},
    {[&]()->string {return "2*x: " + to_string(x*2);}}
  };
  Menu m2menu(&m2);
  m2menu.run();

}
