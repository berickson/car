#include <iostream>

#include "../Beeper.h"
#include "Arduino.h"

using namespace std;
TEST(Beeper,BeeperTest) {
  g_millis = 0;
  cout << "running beeper test" << endl;
  Beeper beeper;
  beeper.attach(22);
  beeper.beep_nbc();
  while(g_millis < 2000) {
    g_millis++;
    beeper.execute(); // execute should always work even if not attached
    cout << ".";
  }
}
