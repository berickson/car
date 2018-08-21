#pragma once
#include "Arduino.h"

// computer steering and speed
class PwmInput {
public:
  unsigned long last_trigger_us = 0;
  unsigned long pulse_width_us = 0;

  // since we are dealing with standard RC,
  // anything out of of the below ranges should not occur and is ignored
  unsigned long max_pulse_us = 2200;
  unsigned long min_pulse_us = 800;

  unsigned long last_pulse_ms = 0;  // time when last pulse occurred


  // milliseconds without a pulse to consider a timeout
  unsigned long timeout_ms = 500;
  int pin;

  void attach(int pin);
  // interrupt handler
  void handle_change();
  bool is_valid();

  // safe method to return pulse width in microseconds
  // returns 0 if invalid
  int pulse_us();
  void trace();
};
