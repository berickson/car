#pragma once

#include "Task.h"

class ManualMode : public Task {
  virtual void begin();
  virtual void end();
  virtual void execute();
};
