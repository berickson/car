#ifndef CARUI_H
#define CARUI_H
#include "pi_buttons.h"

#include <string>

using namespace std;

class CarUI
{
public:
  static PiButtons global_buttons;
  PiButtons & buttons();
  CarUI();
  ~CarUI();

  void init();

  void clear();
  void move(int row, int col);
  void print(string s);
  void refresh();
  int getkey();
  void bold(bool b);

  int wait_key();
  int h,w;

private:
  bool initialized = false;

};

#endif // CARUI_H
