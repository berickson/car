#include "console_menu.h"
#include <iostream>
#include <string>

#include <ncurses.h> // sudo apt-get install libncurses5-dev


void ConsoleMenu::run() {
  // curses initialization
  initscr();
  curs_set(0); // don't display cursor
  noecho(); // don't display characters
  raw();    // read keys immediately
  getmaxyx(stdscr,h,w); // get screen size y,x
  try {
    while(true) {
      display();
      timeout(100);
      int c=getch();

      // many types of enter key
      if(c == KEY_ENTER || c == 10 || c == 13) {
         enter();
      }
      //if(c==27) break;
      if(c=='q') break;
      if(c=='Q') break;

      // get "multi" keys for arrows
      // http://stackoverflow.com/a/11432632/383967
      if(c=='\033') {
        getch(); // eat '['
        c=getch();

        if(c=='A') up();
        if(c=='B') down();
        if(c=='C') enter();
        if(c=='D') escape();
      }

    }
  }
  catch(...) {
    endwin();
    cout << "error caught in menu::run" << endl;
    throw;
  }
  endwin();
}

void ConsoleMenu::display() {
  clear(); // always start by clearing the whole screan
  auto items = current_submenu->items;
  for(unsigned int i=0;i<items.size();++i) {
    move(i,0);
    auto menu_item = items[i];
    string s = menu_item.display_text();

    if(menu_item.action)
      s = s + "*";
    else
      s = s + ">";

    bool selected = (i==current_submenu->current_index);
    if(selected) {
      s = "["+s+"]";
    } else {
      s = " "+s+" ";
    }
    if(selected) attron(A_BOLD);
    printw(s.c_str());
    if(selected) attroff(A_BOLD);
  }
  refresh();
}



#include <ctime>
#include <chrono>

using namespace chrono;


std::string time_string(std::chrono::system_clock::time_point &tp)
{
    auto ttime_t = system_clock::to_time_t(tp);

    std::tm * ttm = localtime(&ttime_t);
    char date_time_format[] = "%Y-%m-%d %H:%M:%S";
    char time_str[100];
    strftime(time_str, 99, date_time_format, ttm);

    return time_str;
}

string time_string() {
  system_clock::time_point t = system_clock::now();

  return time_string(t);
}

void test_console_menu() {
  int x = 1;

  SubMenu m2 = {
    {[&]()->string {return "x: " + to_string(x);}},
    {[&]()->string {return "increment x";},[&](){x++;}},
    {[&]()->string {return "2*x: " + to_string(x*2);}},
    {[&]()->string {return time_string();}}
  };
  ConsoleMenu m2menu(&m2);
  m2menu.run();
}
