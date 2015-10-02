#include "RxEvents.h"
#include "Arduino.h"
#include "EventQueue.h"

void RxEvents::process_pulses(int steer_us, int speed_us) {
  pending.steer = steer_code(steer_us);
  pending.speed = speed_code(speed_us);
  if (! pending.equals(current)) {
    if(++change_count >= change_count_threshold) {
      current = pending;
      change_count = 0;
      recent.add(current);
      new_event = true;
    }
  }
}


 char RxEvents::steer_code(int steer_us) {
  if (steer_us == 0)
    return '?';
  if (steer_us < 1700)
    return 'R';
  if (steer_us > 1300)
    return 'L';
  return 'C';
}

char RxEvents::speed_code(int speed_us) {
  if (speed_us == 0)
    return '?';
  if (speed_us > 1550)
    return 'F';
  if (speed_us < 1450)
    return 'V';
  return 'N';
}

// returns true if new event received since last call
bool RxEvents::get_event() {
  bool rv = new_event;
  new_event = false;
  return rv;
}

void RxEvents::trace() {
  Serial.write(current.steer);
  Serial.write(current.speed);
}
