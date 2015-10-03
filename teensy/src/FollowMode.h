#pragma once

#include "Task.h"

class FollowMode : public Task {
  virtual void begin();
  virtual void end();
  virtual void execute();
};
