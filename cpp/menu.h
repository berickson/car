#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>
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

typedef vector<MenuItem> SubMenu;



class Menu
{
public:
  SubMenu * current_submenu;
  unsigned int current_index = 0;

  Menu(SubMenu * top);

  void up();
  void down();
  void enter();
  void escape();

  virtual void run();
  virtual void display(){}
};


void test_menu();

#endif // MENU_H
