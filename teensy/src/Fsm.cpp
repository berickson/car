#include "Fsm.h"

#include <cstddef>
#include <string.h>

bool equals(const char * p, const char * q) {
  return strcmp(p,q)==0;
}

Fsm::Edge::Edge(const char * _from, const char * _event, const char * _to) {
  from = _from;
  event = _event;
  to = _to;
}

Fsm::Fsm(Task ** _tasks, int _task_count, Edge * _edges, int _edge_count) {
  tasks = _tasks;
  task_count = _task_count;
  edges = _edges;
  edge_count = _edge_count;
  name="fsm";
}

void Fsm::enter() {
  current_task = tasks[0];
  current_task->enter();
}

void Fsm::execute() {
  if(done || current_task == NULL) {
    return;
  }

  current_task->execute();
  
  // see if we have any events to process
  const char * event = current_task->get_event();
  if( event == NULL) {
    return;
  }
  
  // move based on event
  for(int i = 0; i < edge_count; i++) {
    if(equals( edges[i].from, current_task->name ) && equals( edges[i].event, event )) {
      set_current_task(edges[i].to);
      break;
    }
  }
}

void Fsm::set_current_task(const char * name) {
  if(equals(name,"done")) {
    done = true;
    current_task = this;
    return;
  }
  for(int i = 0; i < task_count; i++) {
    Task * task = tasks[i];
    if(equals(task->name,name)){
      current_task = task;
      break;
    }
  }
}

void Fsm::exit() {
  if(current_task && current_task != this) {
    current_task->exit();
  }
  current_task = this;
  done = true;
}

bool Fsm::is_done() {
  return done;;
}

