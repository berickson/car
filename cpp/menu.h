#ifndef MENU_H
#define MENU_H

#include <sstream>
#include <string>
#include <vector>
#include <functional>
#include <list>

using namespace std;


class SubMenu;

struct MenuItem {
  MenuItem(const char *s, function<void()> action = nullptr) {
    this->display_text = [s](){return s;};
    this->action = action;
  }

  MenuItem( const char *s, SubMenu * sub_menu) {
    this->display_text = [s](){return s;};;
    this->sub_menu = sub_menu;
  }

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
  unsigned int display_offset = 0;

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



template<class T> string selection_text(T option, T current) {
  stringstream ss;
  ss << option;

  if (option == current)
    ss << "(*)";
  else
    ss << "( )";
  return ss.str();
}

template <class T> string to_string2(T v){
  stringstream ss;
  ss << v;
  return ss.str();
}

template <class T> void selection_menu(
    SubMenu& s,
    const vector<T>& values,
    function<T()>getter,
    function<void(T)>setter)
{
  for(T value:values) {
    MenuItem m(
      [value,getter](){return selection_text<T>(value,getter());},
      [value,setter](){setter(value);});
    s.items.push_back(std::move(m));
  }
}



void test_menu();

#endif // MENU_H
