#include "Fsm.h"
#include "FakeTask.h"
#include "Arduino.h"

#include <iostream>
using namespace std;

#define count_of(a) (sizeof(a)/sizeof(a[0]))


void PrintFsmState(Fsm & fsm) {
  cout << "fsm state -  done: " << fsm.is_done();
  cout << " current task: " << fsm.current_task->name  << endl;
}

void FsmTest() {
  cout << "FsmTest" << endl;
  FakeTask task1,task2;
  task1.name = "task1";
  task2.name = "task2";
  Task * tasks[] = {&task1, &task2};
  Fsm::Edge edges[] = {{"task1", "ok", "task2"},
                       {"task2", "bad", "task1"},
                       {"task2", "ok", "done"},
                      };
  Fsm fsm(tasks, count_of(tasks), edges, count_of(edges));
  fsm.enter();
  fsm.execute();
  PrintFsmState(fsm);
  
  cout << "no events" << endl;
  fsm.execute();
  PrintFsmState(fsm);

  
  cout << "set bad" << endl;
  fsm.set_event("bad");
  fsm.execute();
  PrintFsmState(fsm);

  cout << "set ok" << endl;
  fsm.set_event("ok");
  fsm.execute();
  PrintFsmState(fsm);

  cout << "set bad" << endl;
  fsm.set_event("bad");
  fsm.execute();
  PrintFsmState(fsm);

  cout << "set nonsense" << endl;
  fsm.set_event("nonsense");
  fsm.execute();
  PrintFsmState(fsm);


  cout << "set ok" << endl;
  fsm.set_event("ok");
  fsm.execute();
  PrintFsmState(fsm);

  cout << "set ok" << endl;
  fsm.set_event("ok");
  fsm.execute();
  PrintFsmState(fsm);

  cout << "execute complete machine" << endl;
  fsm.execute();
  PrintFsmState(fsm);

  
  fsm.exit();
  cout << "test complete"<< endl;
}
