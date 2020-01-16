#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <thread>
#include <unistd.h> // for usleep, etc

#include "ackerman.h"
#include "usb.h"
#include "work-queue.h"
#include "dynamics.h"
#include "config.h"
#include "car.h"
#include "fake-car.h"

#include "string-utils.h"
#include "system.h"
#include "geometry.h"
#include "route.h"
#include "lookup-table.h"
#include "driver.h"
#include "camera.h"
#include "tracker.h"
#include "pid.h"
#include "car-controller.h"
#include "logger.h"

#include <signal.h>

using namespace std;

void handler(int sig) {
  log_error((string) "Error: signal " + to_string(sig));
  log_backtrace();
  exit(1);
}


int main(int, char *[]) {
  signal(SIGSEGV, handler);

  try {
    log_info("enter main");
    // run_car_menu();


    //test_ackerman();
    //test_ackerman2();
    //test_camera();
    //test_dynamics();
    //test_car();
    //test_config();
    //test_console_menu();
    //test_driver();
    //test_dynamics();
    //test_fake_car();
    //test_geometry();
    //test_lookup_table();
    //test_menu();
    //test_pi_buttons();
    //test_pid();
    //test_pose();
    //test_route();
    //test_split();
    //test_stereo_camera(); return 0;
    //test_system();
    //test_tracker();
    //test_usb();

    run_car_socket();

  } catch (string & s) {
    log_error(s);
    log_error("main exiting because of error");
  } catch (...) {
    log_error("main exiting because of error");
  }
  log_info("exit main");
}
