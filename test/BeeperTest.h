#include <iostream>

#include "Beeper.h"

using namespace std;
void BeeperTest() {
  cout << "running beeper test" << endl;
  Beeper beeper;
  beeper.attach(22);
  beeper.beep_nbc();
}
