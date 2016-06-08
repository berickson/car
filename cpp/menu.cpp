#include "menu.h"
#include <list>
#include <iostream>


using namespace std;
Menu::Menu(SubMenu * top) {
  current = top;
}
void Menu::up(){}
void Menu::down(){}
void Menu::enter(){}
void Menu::escape(){}


void Menu::run() {
  for(auto item:*current) {
    cout << item.display_text() << endl;
  }
}


void test_menu() {
  int x = 1;

  SubMenu m2 = {
    {[&]()->string {return "x: " + to_string(x);}},
    {[&]()->string {return "2*x: " + to_string(x*2);}}
  };
  Menu m2menu(&m2);
  m2menu.run();
}
