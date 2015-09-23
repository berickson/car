#include "Beep.h"

#include <cstddef>
#include "Arduino.h"

Beep::Beep(int _pin, int _note, unsigned long _ms) {
  pin = _pin;
  note = _note;
  ms = _ms;
}

void Beep::enter() {
  tone(pin, note);
  start_ms = millis();
}

void Beep::exit() {
  notone(pin);
}

void Beep::execute() {
}

bool Beep::done() {
  return millis() - start_ms > ms;
}
