#include "Sequence.h"
#include "Arduino.h"
#include "Beep.h"

#include <iostream>
using namespace std;

void BeepTest() {
  g_millis = 0;
  Beep beep1(12,20,3), beep2(12,20,4);
  beep1.name = "beep1";
  beep2.name = "beep2";
  State * states[] = {&beep1, &beep2};
  Sequence sequence(states,2);
  sequence.enter();
  for(unsigned long i = 0; i < 1000; i++) {
    g_millis = i;
    cout << "i: " << i << endl;
    cout << "current state: " << sequence.current_state->name << endl;
    sequence.execute();
    if(sequence.done())
        break;
  }
  sequence.exit();

}
