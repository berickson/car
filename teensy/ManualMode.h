#pragma once

#include "Task.h"

class ManualMode : public Task {
public:
  virtual void begin();
  virtual void end();
  virtual void execute();
  ManualMode();
};
