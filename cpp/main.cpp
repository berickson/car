#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <thread>
#include <unistd.h> // for usleep, etc

#include "ackerman.h"
#include "usb.h"
#include "work_queue.h"
#include "dynamics.h"
#include "config.h"
#include "car.h"
#include "fake_car.h"

#include "split.h"
#include "menu.h"
#include "system.h"
#include "car_menu.h"
#include "console_menu.h"
#include "geometry.h"
#include "pi_buttons.h"
#include "route.h"
#include "lookup_table.h"

using namespace std;




int main(int, char *[]) {
  try {
    //test_route();
    //test_pi_buttons();
    //test_geometry();
    //test_car_menu();
    //test_console_menu();
    test_system();
    //test_menu();
    //test_fake_car();
    
    //test_dynamics();
    //test_car();
    //test_usb();
    //test_split();
    //test_config();
    //test_work_queue();
    //test_lookup_table();
  } catch (string & s) {
    cout << "caught: " << s << endl;
  }
}
