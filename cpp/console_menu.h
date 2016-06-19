#ifndef CONSOLEMENU_H
#define CONSOLEMENU_H
#include "menu.h"


class ConsoleMenu : public Menu {
public:
  using Menu::Menu; // inherit constructors
  void run() override;
  void display() override;

  int h,w;
};

void test_console_menu();

#endif // CONSOLEMENU_H
