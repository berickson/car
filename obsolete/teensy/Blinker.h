#pragma once
#include "Arduino.h"
#include "Pins.h"

class Blinker {
public:
  int period_ms = 1000;
  int on_ms = 1;
  int pin = PIN_LED;
  unsigned long wait_ms = 0;
  unsigned long last_change_ms = 0;
  bool is_on = false;

  void init(int _pin = PIN_LED);
  void execute();
};
