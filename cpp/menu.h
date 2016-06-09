#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>
#include <functional>
#include <list>

using namespace std;


class SubMenu;

struct MenuItem {
  MenuItem( function<string()>display_text, function<void()> action = nullptr) {
    this->display_text = display_text;
    this->action= action;
  }

  MenuItem( function<string()>display_text, SubMenu * sub_menu) {
    this->display_text = display_text;
    this->sub_menu = sub_menu;
  }

  function<string()> display_text;
  function<void()> action;
  SubMenu * sub_menu = nullptr;

};

class SubMenu {
public:
  SubMenu(std::initializer_list<MenuItem> l): items(l){;}
  vector<MenuItem> items;
  unsigned int current_index = 0;

};



class Menu
{
public:
  SubMenu * current_submenu;

  Menu(SubMenu * top);

  void up();
  void down();
  void enter();
  void escape();

  virtual void run();
  virtual void display(){}
  list<SubMenu*> parents;
};


void test_menu();

#endif // MENU_H
