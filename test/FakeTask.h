#pragma once
#include "Task.h"

class FakeTask : public Task {
public:
  bool done = false;
};
