#include "pi_buttons.h"
#include <wiringPi.h>
#include <unistd.h> // for usleep

#include <iostream>

using namespace std;

void test_pi_buttons() {
  wiringPiSetup();
  pinMode(1,INPUT);
  pullUpDnControl(1,PUD_UP);
  pinMode(2,INPUT);
  pullUpDnControl(2,PUD_UP);
  pinMode(3,INPUT);
  pullUpDnControl(3,PUD_UP);
  pinMode(4,INPUT);
  pullUpDnControl(4,PUD_UP);
  while(true) {
    cout << digitalRead(1) << digitalRead(2) << digitalRead(3) << digitalRead(4) << endl;
    usleep(1E5); // 0.1 second
  }
}
