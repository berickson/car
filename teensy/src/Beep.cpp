#include "Beep.h"

#include <cstddef>
#include "Arduino.h"

Beep::Beep() {
 pin = note = ms = -1;
}

void Beep::init(int _pin, int _note, unsigned long _ms) {
  pin = _pin;
  note = _note;
  ms = _ms;
  playing = false;
}

void Beep::enter() {
  tone(pin, note);
  start_ms = millis();
  playing = true;
}

void Beep::exit() {
  stop();
}

void Beep::execute() {
   if(playing && is_done())
     stop();
}

void Beep::stop() {
   if(playing) {
     noTone(pin);
     playing = false;
   }
}

bool Beep::is_done() {
  return millis() - start_ms > ms;
}
