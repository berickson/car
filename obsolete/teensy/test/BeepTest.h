#include "Sequence.h"
#include "Arduino.h"
#include "Beep.h"

#include <iostream>
using namespace std;

TEST(Beep, Beep) {
  g_millis = 0;
  Beep beep1, beep2;
  beep1.init(12,20,3);
  beep2.init(12,20,4);
  beep1.name = "beep1";
  beep2.name = "beep2";
  Sequence sequence;
  sequence.init();
  sequence.add_task(&beep1);
  sequence.add_task(&beep2);
  sequence.begin();
  for(unsigned long i = 0; i < 1000; i++) {
    g_millis = i;
    cout << "i: " << i << endl;
    cout << "current task: " << sequence.current_task->name << endl;
    sequence.execute();
    if(sequence.is_done())
        break;
  }
  sequence.end();

}
