#include "console_menu.h"
#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include "car_ui.h"
#include <unistd.h> // usleep

void ConsoleMenu::run() {
  ui.init();
  // curses initialization

  try {
    while(true) {
      display();
      usleep(30000);

      {
        char p = ui.buttons().get_press();
        if(p=='2') up();
        if(p=='3') down();
        if(p=='4') enter();
        if(p=='1') escape();

      }

      int c=ui.getkey();

      // many types of enter key
      if( c == 10 || c == 13) {
         enter();
      }
      //if(c==27) break;
      if(c=='q') break;
      if(c=='Q') break;

      // get "multi" keys for arrows
      // http://stackoverflow.com/a/11432632/383967
      if(c=='\033') {
        ui.getkey(); // eat '['
        c=ui.getkey();

        if(c=='A') up();
        if(c=='B') down();
        if(c=='C') enter();
        if(c=='D') escape();
      }

    }
  }
  catch(string &e) {
    cout << "cought error string: " << e << endl;
    throw(e);
  }
  catch(...) {
    cout << "error caught in menu::run" << endl;
    throw;
  }
}

void ConsoleMenu::display() {
  ui.clear(); // always start by clearing the whole screan

  // see if current item fits on screen, adjust offset if necessary
  while( ((int)current_submenu->current_index - (int)current_submenu->display_offset) > (ui.h-1) ) {
    ++(current_submenu->display_offset);
  }
  // move up if it is off the top
  while(current_submenu->current_index < current_submenu->display_offset) {
    --(current_submenu->display_offset);
  }



  auto items = current_submenu->items;
  for(unsigned int i=current_submenu->display_offset;i<items.size();++i) {
    int line = i-current_submenu->display_offset;
    if(line >= ui.h) break;

    ui.move(line,0);
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
    //if(selected) ui.bold(true);
    ui.bold(true);
    ui.print(s);
    //if(selected) ui.bold(false);
  }
  ui.refresh();
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
