#include "Sequence.h"

#include <cstddef>

Sequence::Sequence(State ** _states, int _state_count) {
  states = _states;
  state_count = _state_count;
}

void Sequence::enter() {
  current_step = 0;
  current_state = states[current_step];
  current_state->enter();
}

void Sequence::execute() {
  if(current_state == NULL) {
    return;
  }

  current_state->execute();
  if(current_state->done()){
    current_state->exit();
    current_step++;
    if(current_step < state_count) {
      current_state = states[current_step];
      current_state->enter();
    } else {
      current_state = NULL;
    }
  }
}

void Sequence::exit() {
  if(current_state) {
    current_state->exit();
  }
  current_state = NULL;
  current_step = -1;
}

bool Sequence::done() {
  return current_state == NULL;
}

