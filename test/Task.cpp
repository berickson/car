#include "Task.h"

#include <cstddef>
#include "Arduino.h"

void Task::enter() {}
void Task::exit() {}
void Task::execute() {
}
void Task::set_event(const char * _event) {
  event = _event;
}
const char * Task::get_event() {
  return event;
}

bool Task::is_done() {
  return done;
}
