#pragma once

class Task {
public:
  const char * name = 0;
  bool done = false;
  virtual void enter();
  virtual void exit();
  virtual void execute();
  virtual bool is_done();
  void set_event(const char * event);
  const char * get_event();
private:
  const char * event;
};