#pragma once

#include <cstddef>
#include "State.h"
class Sequence : public State {
  public:
  State ** states;
  int state_count = -1;
  int current_step = -1;
  State * current_state = NULL;

  Sequence(State ** _states, int _state_count);
  void enter();
  void execute();
  bool done();
  void exit();
};
