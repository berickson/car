#ifndef CARUI_H
#define CARUI_H

#include "ncurses.h"
#include <string>

using namespace std;

class CarUI
{
public:
  CarUI();

  inline void clear() {
    ::clear();
  }

  inline void move(int row, int col) {
    ::move(row,col);
    refresh();

  }

  inline void print(string s) {
    printw(s.c_str());
  }

  inline void refresh() {
    ::refresh();
  }

  inline int getkey() {
    int k = getch();
    return k;
  }

  inline void wait_key() {
    while(getch()==1) {}
    return;
  }


};

#endif // CARUI_H
