#include "Task.h"

#include <cstddef>
#include "Arduino.h"

void Task::begin() {}
void Task::end() {}
void Task::execute() {
}


bool Task::is_done() {
  return done;
}
