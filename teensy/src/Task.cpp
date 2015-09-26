#include "Task.h"

#include <cstddef>
#include "Arduino.h"

void Task::enter() {}
void Task::exit() {}
void Task::execute() {
}


bool Task::is_done() {
  return done;
}
