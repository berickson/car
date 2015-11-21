#pragma once

#include <cstddef>
#include "Task.h"

class Fsm : public Task {
  public:
  // TBD: use numbers instead of strings?
  struct Edge {
    Edge(const char * from, const char * event, const char *);
    const char * from = NULL;
    const char * event = NULL;
    const char * to = NULL;
  };
  Edge * edges = NULL;
  int edge_count = -1;
  Task ** tasks = NULL;
  int task_count = -1;
  Task * current_task = NULL;
  bool done = false;
  void set_current_task(const char * name);

  Fsm(Task ** _tasks, int _task_count, Edge * _edges, int _edge_count);
  void begin();
  void execute();
  bool is_done();
  void end();
  void set_event(const char * event);
};
