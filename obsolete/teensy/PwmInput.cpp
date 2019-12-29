#include "Arduino.h"
#include "PwmInput.h"

void PwmInput::attach(int pin) {
  this->pin = pin;
  last_trigger_us = 0;
  pulse_width_us = 0;
  pinMode(pin, INPUT);
}

// interrupt handler
void PwmInput::handle_change() {
  unsigned long us = micros();
  if(digitalRead(pin)) {
    last_trigger_us = us;
  }
  else {
    unsigned long width = us - last_trigger_us;
    // only accept pulses in acceptable range
    if(width >= min_pulse_us && width <= max_pulse_us) {
      pulse_width_us = width;
      last_pulse_ms = millis();
    }
  }
}

bool PwmInput::is_valid() {
  return millis() - last_pulse_ms < timeout_ms;
}

// safe method to return pulse width in microseconds
// returns 0 if invalid
int PwmInput::pulse_us() {
  if(is_valid())
    return pulse_width_us;
  else
    return 0;
}

void PwmInput::trace() {
  Serial.print(pin);
  Serial.print(" ");
  Serial.print(pulse_width_us);
  if(is_valid())
    Serial.print(" valid");
  else
    Serial.print(" invalid");
}
