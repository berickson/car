#pragma once
#include "Task.h"

class Beep : public Task {
public:
  int pin;
  int note;
  bool playing = false;
  unsigned long ms;
  unsigned long start_ms;
  Beep();
  void init(int pin, int note, unsigned long ms);
  
  virtual void begin();
  virtual void end();
  virtual void execute();
  virtual bool is_done();
  
private:
  void stop();
};
