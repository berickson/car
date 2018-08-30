#pragma once

#include "RxEvent.h"
#include "EventQueue.h"

// generates RxEvents based on receiver pulsewidths
class RxEvents {
public:
  RxEvent current, pending;
  bool new_event = false;
  EventQueue recent;

  // number of different readings before a new event is triggered
  // this is used to de-bounce the system
  const int change_count_threshold = 5;
  int change_count = 0;

  void process_pulses(int steer_us, int speed_us);
  char steer_code(int steer_us);

  char speed_code(int speed_us);

  // returns true if new event received since last call
  bool get_event();
  void trace();
};
