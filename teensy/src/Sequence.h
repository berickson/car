#pragma once

#include <cstddef>
#include "Task.h"
class Sequence : public Task {
  public:
  static const int max_tasks = 50;
  Task * tasks[max_tasks];
  int task_count = -1;
  int current_step = -1;
  Task * current_task = NULL;
  Sequence();
  void init();
  void add_task(Task * task);
  void enter();
  void execute();
  void exit();
  bool is_done();
};
