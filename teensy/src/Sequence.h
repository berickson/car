#pragma once

#include <cstddef>
#include "Task.h"
class Sequence : public Task {
  public:
  Task ** tasks;
  int task_count = -1;
  int current_step = -1;
  Task * current_task = NULL;

  Sequence(Task ** _tasks, int _task_count);
  void enter();
  void execute();
  void exit();
  bool is_done();
};
