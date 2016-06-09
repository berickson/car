#include "menu.h"
#include <list>
#include <iostream>


using namespace std;
Menu::Menu(SubMenu * top) {
  current_submenu = top;
}
void Menu::up() {
  if(current_submenu->current_index > 0) {
    --current_submenu->current_index;
  }
}

void Menu::down() {
  if(current_submenu->current_index+1 < current_submenu->items.size()){
    ++current_submenu->current_index;
  }
}

void Menu::enter() {
  if(current_submenu->items[current_submenu->current_index].action) {
    current_submenu->items[current_submenu->current_index].action();
  }
  if(current_submenu->items[current_submenu->current_index].sub_menu) {
    parents.push_back(current_submenu);
    current_submenu = current_submenu->items[current_submenu->current_index].sub_menu;
  }
}

void Menu::escape() {
  if(parents.size()){
    current_submenu = parents.back();
    parents.pop_back();
  }
}

void Menu::run() {
  for(auto item:current_submenu->items) {
    cout << item.display_text() << endl;
  }
}

