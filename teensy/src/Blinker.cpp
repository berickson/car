#include "Blinker.h"

void Blinker::init(int _pin) {
  pin = _pin;
  pinMode(pin, OUTPUT);
}

void Blinker::execute() {
  unsigned long ms = millis();
  if(ms - last_change_ms <= wait_ms)
    return;
  is_on = !is_on;
  last_change_ms = ms;
  digitalWrite(pin, is_on);
  if(is_on) {
    wait_ms =on_ms;
  }
  else {
    wait_ms = period_ms - on_ms;
  }
}
