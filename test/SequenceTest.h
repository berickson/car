#include "Sequence.h"
#include "Arduino.h"

#include <iostream>
using namespace std;


void SequenceTest() {
  cout << "SequenceTest" << endl;
  Task task1,task2;
  task1.name = "task1";
  task2.name = "task2";
  Task * tasks[] = {&task1, &task2};
  Sequence sequence(tasks,2);

  sequence.enter();
  for(int i = 0; i < 1000; i++) {
    cout << "i: " << i << endl;
    cout << "current task: " << sequence.current_task->name << endl;
    sequence.execute();
    if(sequence.is_done())
        break;
  }

  sequence.exit();
}
