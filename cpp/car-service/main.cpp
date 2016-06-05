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

#include "split.h"

using namespace std;





class Printer {
public:
    string text;
    void run() {
        cout << text << endl;
    }
};

int main(int, char *[])
{
  test_usb();
  return 0;

  test_split();
  return 0;

  test_car();
  return 0;

  test_config();
  return 0;

  test_dynamics();
  return 0;

  test_work_queue();
  return 0;

  Usb usb;
  usb.run();
  return 0;

  move_left_wheel_tests();
  arc_to_relative_location_tests();
  return 0;

  string s = "TD,939.27,-32,Heading String,5";
  cout << "splitting string " << s << endl;
  for(auto i: split(s)){
      cout << i << endl;
  }


  Printer p;
  p.text = "calling a member from a thread";
  thread t1(&Printer::run, p);
  t1.join();
  return 0;
}
