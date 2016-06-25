#include "car_ui.h"
#include "ncurses.h"
#include <unistd.h> // usleep

PiButtons CarUI::global_buttons;

PiButtons &CarUI::buttons() {
  return CarUI::global_buttons;
}

CarUI::CarUI() {

}

CarUI::~CarUI()
{
  if(initialized)
    endwin();
}

void CarUI::init()
{
  if(initialized)
    return;
  initscr();
  curs_set(0); // don't display cursor
  noecho(); // don't display characters
  raw();    // read keys immediately
  getmaxyx(stdscr,h,w); // get screen size y,x
  timeout(100); // timeout for key presses in ms
  initialized = true;
}

void CarUI::clear() {
  ::clear();
}

void CarUI::move(int row, int col) {
  ::move(row,col);
  refresh();

}

void CarUI::print(string s) {
  printw(s.c_str());
}

void CarUI::refresh() {
  ::refresh();
}

int CarUI::getkey() {
  int k = getch();
  return k;
}

void CarUI::bold(bool b)
{
  if(b)
    attron(A_BOLD);
  else
    attroff(A_BOLD);
}

void CarUI::wait_key() {
  while(getch()==-1 && buttons().get_press()==0) {
    usleep(1000);
  }
  return;
}
