#ifndef CONSOLEMENU_H
#define CONSOLEMENU_H
#include "menu.h"
#include "car_ui.h"


class ConsoleMenu : public Menu {
public:
  using Menu::Menu; // inherit constructors
  void run() override;
  void display() override;
  CarUI ui;
};

void test_console_menu();

#endif // CONSOLEMENU_H
