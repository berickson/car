#include "Sequence.h"

#include <cstddef>

Sequence::Sequence() {
  task_count = -1;
  current_step = -1;
  current_task = NULL;
}

void Sequence::init() {
  exit(); // in case were are in the middle of something
  task_count = 0;
}

void Sequence::add_task(Task * task) {
  // silently fail if there isn't room for one more
  if(task_count+1 >= max_tasks) {
    return;
  }
  tasks[task_count] = task;
  task_count++;
}

void Sequence::enter() {
  current_step = 0;
  current_task = tasks[current_step];
  current_task->enter();
}

void Sequence::execute() {
  if(current_task == NULL) {
    return;
  }

  current_task->execute();
  if(current_task->is_done()){
    current_task->exit();
    current_step++;
    if(current_step < task_count) {
      current_task = tasks[current_step];
      current_task->enter();
    } else {
      current_task = NULL;
      done = true;
    }
  }
}

void Sequence::exit() {
  if(current_task) {
    current_task->exit();
  }
  current_task = NULL;
  current_step = -1;
}

bool Sequence::is_done() {
  return current_task == NULL;
}

