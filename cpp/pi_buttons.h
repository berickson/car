#ifndef PI_BUTTONS_H
#define PI_BUTTONS_H

#include <string>
#include <vector>

using namespace std;
class PiButtons {
public:
  PiButtons();
  char get_press();

  vector<bool> pressed = {false,false,false,false,false}; // 0-4, but we only use 1..4

  bool wiring_ok = false;
private:
  bool check_press(int button);
};


void test_pi_buttons();

#endif // PI_BUTTONS_H
