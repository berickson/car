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

using namespace std;




int main(int, char *[]) {
  test_fake_car();
  //test_dynamics();
  //test_car();
  //test_usb();
  //test_split();
  //test_config();
  //test_work_queue();
}
