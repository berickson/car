#pragma once

#include "Task.h"

class RemoteMode : public Task {
public:
  RemoteMode();
  virtual void begin();
  virtual void end();
  virtual void execute();
  
  void command_steer_and_esc(float str_us, float esc_us);
private:
  bool is_active = false;
  void update_pulses();
  unsigned long last_command_ms = 0;
  float str_us = 1500;
  float esc_us = 1500;
};
