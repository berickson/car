#include "pi_buttons.h"
#include <wiringPi.h>
#include <unistd.h> // for usleep

#include <iostream>

using namespace std;

void test_pi_buttons() {
  wiringPiSetup();
  pinMode(1,INPUT);
  digitalWrite(1,HIGH);
  pinMode(2,INPUT);
  digitalWrite(2,HIGH);
  pinMode(3,INPUT);
  digitalWrite(3,HIGH);
  pinMode(4,INPUT);
  digitalWrite(4,HIGH);
  while(true) {
    cout << digitalRead(1) << digitalRead(2) << digitalRead(3) << digitalRead(4) << endl;
    usleep(1E5); // 0.1 second
  }
}
