#!/usr/bin/env python2.7
# coding: utf-8

import time
import curses

class Menu:
  stack = []
  items = ['3.2v ok menu>','192.168.1.6','item2','item3','item4']
  position = 0
  w = 16
  _quit = False
  line1 = 'menu line 1'
  line2 = 'menu line 2'
  def process_key(self,k):
    if c == ord('q'):
      self._quit = True
    if c == curses.KEY_UP:
      self.position = self.position - 1
      self.position = max(self.position,0)
    if c == curses.KEY_DOWN:
      self.position = self.position + 1
      self.position = min(self.position,len(self.items)-1)
    if c == curses.KEY_RIGHT:
      self.stack.append(self.position)
      self.position = 0
    if c == curses.KEY_LEFT:
      if len(self.stack) > 0:
        self.position = self.stack.pop()

  def get_item(self, p):
    if p < 0 or p > len(self.items)-1:
      return ''
    else:
      return self.items[p]
    
  def clip(self,s):
    return s[:self.w].ljust(self.w)

  def text1(self):
    return self.clip(self.get_item(self.position))

  def text2(self):
    return self.clip(self.get_item(self.position+1))

  def quit(self):
    return self._quit


w=16+2
h=2+2
x=2
y=2
stdscr = curses.initscr()
try:
  curses.noecho()
  curses.cbreak() # read keys instantly
  win = curses.newwin(h,w,y,x)
  stdscr.keypad(1)
  curses.curs_set(0)
  win.addstr(0,0,"hello")
  win.box()
  stdscr.refresh()
  win.refresh()
  menu = Menu()
  while not menu.quit():
    win.addstr(1,1,menu.text1())
    win.addstr(2,1,menu.text2())
    win.refresh()
    c = stdscr.getch()
    menu.process_key(c)
      
finally:
  curses.nocbreak()
  stdscr.keypad(0)
  curses.echo()
  curses.endwin()
  curses.curs_set(1)

