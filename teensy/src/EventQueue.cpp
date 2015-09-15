#include "EventQueue.h"
#include "Arduino.h"
#include "LogFlags.h"


extern bool TRACE_RX;

void EventQueue::add(RxEvent new_event) {
  for(int i = size-1; i > 0; --i) {
    events[i]=events[i-1];
  }
  events[0] = new_event;

  if(TRACE_RX) {
    for(int i = 0; i < size; i++) {
      Serial.print(events[i].steer);
      Serial.print(events[i].speed);
      Serial.print(",");
    }
    Serial.println();
  }
}

bool EventQueue::matches(const RxEvent * pattern, int count) {
  for(int i = 0; i < count; ++i) {
    if(!events[i].equals(pattern[count-1 - i])) {
      return false;
    }
  }
  return true;
}
