#!/usr/bin/env python2.7
# coding: utf-8

import time
import curses
import numpy as np
import pdb
from lcd import LCD, Lcd
#from car import Car

class FakeLcd:
  def __init__(self,h=2,w=16,x=10,y=10):
    self.h=h
    self.w=w
    self.x=10
    self.y=10
    
    self.stdscr = curses.initscr()
    curses.noecho()
    curses.cbreak() # read keys instantly
    self.win = curses.newwin(self.h+2,self.w+2,self.y,self.x)
    self.stdscr.keypad(1)
    curses.curs_set(0)
    self.win.addstr(0,0,"hello")
    self.win.box()
    self.stdscr.refresh()
    self.win.refresh()
    self.stdscr.nodelay(1)
    
  # returns a curses key press as an LCD.* constant.  
  # Returns None if no key pressed.
  def getch(self):
    c = self.stdscr.getch()
    if c == curses.KEY_UP:
      return LCD.UP
    if c == curses.KEY_DOWN:
      return LCD.DOWN
    if c == curses.KEY_RIGHT:
      return LCD.RIGHT
    if c == curses.KEY_LEFT:
      return LCD.LEFT
    if c ==  ord(' '):
      return LCD.SELECT
    return None

  def __del__(self):
    curses.nocbreak()
    self.stdscr.keypad(0)
    curses.echo()
    curses.endwin()
    curses.curs_set(1)
  
  def display_text(self,s):
    lines = s.split('\n')
    for i in range(self.h):
      if len(lines)<i : break
      self.win.addstr(i+1,1, lines[i])
    self.win.refresh()

class Config:
  pass
config = Config()
config.max_a = 1.0
config.max_v = 2.0


class MenuItem:
  _text = ''
  sub_menu = None
  action = None
  
  def __init__(self,text, sub_menu = None, action = None):
    self._text = text
    self.sub_menu = sub_menu
    self.action = action
  def __str__(self):
    s = ''
    if hasattr(self._text, '__call__'):
      s = self._text()
    else:
      s= str(self._text)
    return s

def selection_text(option,current):
  s=str(option)
  if option == current:
    s += "(*)"
  else:
    s += "( )"
  return s

# getter / setter methods to help as actions in menus
def max_a(a = None):
  if a is not None:
    config.max_a = a
  return config.max_a

def max_v(v = None):
  if v is not None:
    config.max_v = v
  return config.max_v
  
sub1 = [MenuItem(lambda:time.strftime('%H:%M:%S'))];


def selection_menu(value_function, values):
  return  map(
    lambda a: MenuItem(
      lambda: selection_text(a,value_function()),
      action=lambda:value_function(a)),
    values)

acceleration_menu = selection_menu(max_a, np.arange(0.1,3,0.1))
velocity_menu = selection_menu(max_v,np.arange(1,30,1))
monitor_menu = [MenuItem('esc')]


main_menu = [
    MenuItem('3.2v ok menu',sub_menu = sub1),
    MenuItem('Record'),
    MenuItem('Monitor',sub_menu = monitor_menu),
    MenuItem('Route'),
    MenuItem('192.168.1.6'),
    MenuItem(lambda:'max_a[{}]'.format(config.max_a),sub_menu = acceleration_menu ),
    MenuItem(lambda:'max_v[{}]'.format(config.max_v),sub_menu = velocity_menu)]


class Menu:
  stack = []
  items = []
    
  position = 0
  w = 16
  h = 2
  x = 2
  y = 2
  _quit = False
  
  def __init__(self,items):
    self.items = items
    
  def process_key(self,c):
#    if c == ord('q'):
#      self._quit = True
    if c == LCD.SELECT:
      item =  self.current_item()
      if item.action is not None:
        item.action()
    if c == LCD.UP:
      self.position = self.position - 1
      self.position = max(self.position,0)
    if c == LCD.DOWN:
      self.position = self.position + 1
      self.position = min(self.position,len(self.items)-1)
    if c == LCD.RIGHT:
      item =  self.current_item()
      if item.sub_menu is None:
        return
      self.stack.append((self.items,self.position))
      self.items = self.get_item(self.position).sub_menu
      self.position = 0
    if c == LCD.LEFT:
      if len(self.stack) > 0:
        self.items,self.position = self.stack.pop()
        
  def current_item(self):
    return self.get_item(self.position)

  def get_item(self, p):
    if p < 0 or p > len(self.items)-1:
      return MenuItem('')
    else:
      return self.items[p]
    
  def clip(self,s):
    return s[:self.w].ljust(self.w)
    
  def text_at(self,p):
    menuItem = self.get_item(p)
    s = str(self.get_item(p))
    if menuItem.sub_menu is not None and p == self.position:
      s = s + ' >'
    return self.clip(s)

  def text1(self):
    return self.text_at(self.position)

  def text2(self):
    return self.text_at(self.position+1)

  def quit(self):
    return self._quit
    
  def show_in_terminal(self):
    try:
      fake_lcd = FakeLcd()
      lcd = Lcd()
      while not False:# self.quit():
        t = self.text1()+"\n"+self.text2()
        fake_lcd.display_text(t)
        lcd.display_text(t)
        c = lcd.getch()
        if c is None:
          c = fake_lcd.getch()
        if c is not None:
          self.process_key(c)
        time.sleep(0.03)
    finally:
      del lcd

menu = Menu(main_menu)
menu.show_in_terminal()

