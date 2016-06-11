#include "pi_buttons.h"
#include <wiringPi.h>
#include <unistd.h> // for usleep

#include <iostream>

using namespace std;

PiButtons::PiButtons() {
#ifdef RASPBERRY_PI
  try {
    if(piBoardRev()==-1) {
      cout << "not a pi" << endl;
      return;
    }
    if(wiringPiSetup()!=0)
       return;
    pinMode(1,INPUT);
    pullUpDnControl(1,PUD_UP);
    pinMode(2,INPUT);
    pullUpDnControl(2,PUD_UP);
    pinMode(3,INPUT);
    pullUpDnControl(3,PUD_UP);
    pinMode(4,INPUT);
    pullUpDnControl(4,PUD_UP);

    wiring_ok = true;
  } catch (...) {
    wiring_ok = false;
  }
#endif

}

// returns true if there is a new key press
bool PiButtons::check_press(int i) {
  bool down = digitalRead(i) == 0;
  bool just_pressed = !pressed[i] && down;
  pressed[i] = down;
  return just_pressed;
}

char PiButtons::get_press() {
  if(!wiring_ok)
    return 0;

  if(check_press(1)) return '1';
  if(check_press(2)) return '2';
  if(check_press(3)) return '3';
  if(check_press(4)) return '4';
  return 0;
}


void test_pi_buttons() {
  cout << "testing pi buttons" << endl;
  PiButtons buttons;
  while(true) {
    char c = buttons.get_press();
    if(c)
      cout << c << endl;
    usleep(5000); // 0.1 second
  }
}
