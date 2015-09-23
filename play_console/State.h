#pragma once

class State {
public:
  const char * name = 0;
  virtual void enter();
  virtual void exit();
  virtual void execute();
  virtual bool done();
};
