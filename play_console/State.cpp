#include "State.h"

#include <cstddef>
#include "Arduino.h"

void State::enter() {}
void State::exit() {}
void State::execute() {
}
bool State::done() {
  return true;
}
