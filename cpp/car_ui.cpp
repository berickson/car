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
  timeout(0); // timeout for key presses in ms
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

// reads a key, returns key on success or -1 on error
int CarUI::getkey() {
  int k;
  k = buttons().get_press();
  if(k > 0) {
    return k;
  }
  k = getch();
  return k;
}

void CarUI::bold(bool b)
{
  if(b)
    attron(A_BOLD);
  else
    attroff(A_BOLD);
}

int CarUI::wait_key() {
  while(true) {
  int c = getkey();
    if(c!= -1) {
      return c;
    }
    usleep(1000);
  }\
}
