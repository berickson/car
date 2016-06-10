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

using namespace std;




int main(int, char *[]) {
  try {
    test_car_menu();
    //test_console_menu();
    // test_car_menu();
    //test_system();
    //test_menu();
    //test_menu();
    //test_fake_car();
    
    //test_dynamics();
    //test_car();
    //test_usb();
    //test_split();
    //test_config();
    //test_work_queue();
  } catch (string & s) {
    cout << "caught: " << s << endl;
  }
}
