#include "Sequence.h"

#include <iostream>
using namespace std;


void SequenceTest() {
  State state1,state2;
  state1.name = "state1";
  state2.name = "state2";
  State * states[] = {&state1, &state2};
  Sequence sequence(states,2);

  sequence.enter();
  for(int i = 0; i < 1000; i++) {
    cout << "i: " << i << endl;
    cout << "current state: " << sequence.current_state->name << endl;
    sequence.execute();
    if(sequence.done())
        break;
  }

  sequence.exit();  
}
