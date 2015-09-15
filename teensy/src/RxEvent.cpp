#include "RxEvent.h"

RxEvent::RxEvent(char steer, char speed) {
  this->steer = steer;
  this->speed = speed;
}

bool RxEvent::equals(RxEvent other) {
  return speed == other.speed && steer == other.steer;
}

bool RxEvent::is_bad(){
  return speed == '?' || steer == '?';
}
