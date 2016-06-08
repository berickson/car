#ifndef MENU_H
#define MENU_H

#include <string>
#include <list>
#include <functional>

using namespace std;

struct MenuItem {
  MenuItem( function<string()>display_text, function<void()> action = nullptr) {
    this->display_text = display_text;
    this->action= action;
  }

  function<string()> display_text;
  function<void()> action;

};

typedef list<MenuItem> SubMenu;



class Menu
{
public:
  SubMenu * current;
  Menu(SubMenu * top);

  void up();
  void down();
  void enter();
  void escape();

  void run();
};

void test_menu();

#endif // MENU_H
