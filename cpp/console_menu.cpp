#include "console_menu.h"
#include <iostream>
#include <string>
#include <sstream>
#include <functional>

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

template<class T> string selection_text(T option, T current) {
  stringstream ss;
  ss << option;

  if (option == current)
    ss << "(*)";
  else
    ss << "( )";
  return ss.str();
}

template <class T> void selection_menu(
    SubMenu& s,
    const list<T>& values,
    function<T()>getter,
    function<void(T)>setter)
{
  for(T value:values) {
    MenuItem m(
      [value,&getter](){return selection_text(value,getter());},
      [value,&setter](){setter(value);});
    s.items.push_back(m);
  }
}

void ConsoleMenu::display() {
  clear(); // always start by clearing the whole screan
  auto items = current_submenu->items;
  for(unsigned int i=0;i<items.size();++i) {
    move(i,0);
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
  selection_menu<string>(letters, list<string>({"a","b","c"}),get_letter,set_letter);
  SubMenu numbers({});
  selection_menu<double>(numbers, list<double>({3,4,5.5}),get_number,set_number);

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
