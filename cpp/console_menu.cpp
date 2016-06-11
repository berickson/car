#include "console_menu.h"
#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include "pi_buttons.h"

#include <ncurses.h> // sudo apt-get install libncurses5-dev


void ConsoleMenu::run() {
  // curses initialization
  initscr();
  curs_set(0); // don't display cursor
  noecho(); // don't display characters
  raw();    // read keys immediately
  getmaxyx(stdscr,h,w); // get screen size y,x
  PiButtons buttons;
  try {
    while(true) {
      display();
      timeout(100);
      int c=getch();
      if(c==0) {
        c=buttons.get_press();
      }

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

        if(c=='A' || c=='2') up();
        if(c=='B' || c=='3') down();
        if(c=='C' || c=='4') enter();
        if(c=='D' || c=='1') escape();
      }

    }
  }
  catch(string &e) {
    endwin();
    cout << "cought error string: " << e << endl;
    throw(e);
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

  // see if current item fits on screen, adjust offset if necessary
  while( ((int)current_submenu->current_index - (int)current_submenu->display_offset) > (h-1) ) {
    ++(current_submenu->display_offset);
  }
  // move up if it is off the top
  while(current_submenu->current_index < current_submenu->display_offset) {
    --(current_submenu->display_offset);
  }



  auto items = current_submenu->items;
  for(unsigned int i=current_submenu->display_offset;i<items.size();++i) {
    int line = i-current_submenu->display_offset;
    if(line >= h) break;

    move(line,0);
    auto menu_item = items[i];
    string s = menu_item.display_text();

    if(menu_item.sub_menu)
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

string letter = "a";
string get_letter() {
  return letter;
}
void set_letter(string s) {
  letter = s;
}

double number = 5;
double get_number() {
  return number;
}
void set_number(double v) {
  number = v;
}



void test_console_menu() {
  int x = 1;

  SubMenu letters({});
  selection_menu<string>(letters, vector<string>({"a","b","c"}),get_letter,set_letter);
  SubMenu numbers({});
  selection_menu<double>(numbers, vector<double>({3,4,5.5}),get_number,set_number);

  SubMenu child = {
    {[&]()->string {return "line 1";}},
    {[&]()->string {return "line 2";}},
    {[&]()->string {return "line 3";}},
  };

  SubMenu top = {
    {[&]()->string {return "x: " + to_string(x);}},
    {[&]()->string {return "increment x";},[&](){x++;}},
    {[&]()->string {return "2*x: " + to_string(x*2);}},
    {[&]()->string {return "child";},&child},
    {[&]()->string {return "letter";},&letters},
    {[&]()->string {return "number";},&numbers},
    {[&]()->string {return time_string();}}
  };
  ConsoleMenu m2menu(&top);
  m2menu.run();
}
