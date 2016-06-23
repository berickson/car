#ifndef CARUI_H
#define CARUI_H

#include <string>

using namespace std;

class CarUI
{
public:
  CarUI();
  ~CarUI();

  void init();

  void clear();
  void move(int row, int col);
  void print(string s);
  void refresh();
  int getkey();
  void bold(bool b);

  void wait_key();
  int h,w;

private:
  bool initialized = false;

};

#endif // CARUI_H
