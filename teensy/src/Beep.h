#pragma once
#include "Task.h"

class Beep : public Task {
public:
  int pin;
  int note;
  unsigned long ms;
  unsigned long start_ms;
  Beep(int pin, int note, unsigned long ms);
  virtual void enter();
  virtual void exit();
  virtual void execute();
  virtual bool done();
};
