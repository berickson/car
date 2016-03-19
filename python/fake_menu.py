#!/usr/bin/env python2.7
# coding: utf-8

import time
import curses
import numpy as np



class Config:
  pass
config = Config()
config.max_a = 1.0
config.max_v = 2.0


class MenuItem:
  _text = ''
  childMenu = None
  action = None
  selected = None
  
  def __init__(self,text, childMenu = None, action = None, selected = None):
    self._text = text
    self.childMenu = childMenu
    self.action = action
    self.selected = selected
  def __str__(self):
    s = ''
    if hasattr(self._text, '__call__'):
      s = self._text()
    else:
      s= str(self._text)
    if self.selected is not None:
      if self.selected == False:
        s += "()"
      else:
        s += "(*)"
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
acceleration_menu = map(lambda a: MenuItem(lambda: selection_text(a,max_a()),action=lambda:max_a(a)),np.arange(0.1,3,0.1))
velocity_menu = map(lambda v: MenuItem(lambda: selection_text(v,max_v()),action=lambda:max_v(v)),np.arange(1,30,1))
monitor_menu = [MenuItem('esc')]

class Menu:
  stack = []
  items = [
    MenuItem('3.2v ok menu',childMenu = sub1),
    MenuItem('Record'),
    MenuItem('Monitor',childMenu = monitor_menu),
    MenuItem('Route'),
    MenuItem('192.168.1.6'),
    MenuItem(lambda:'max_a[{}]'.format(config.max_a),childMenu = acceleration_menu ),
    MenuItem(lambda:'max_v[{}]'.format(config.max_v),childMenu = velocity_menu)]

    
  position = 0
  w = 16
  _quit = False
  def process_key(self,k):
    if c == ord('q'):
      self._quit = True
    if c == ord(' '):
      item =  self.current_item()
      if item.action is not None:
        item.action()
    if c == curses.KEY_UP:
      self.position = self.position - 1
      self.position = max(self.position,0)
    if c == curses.KEY_DOWN:
      self.position = self.position + 1
      self.position = min(self.position,len(self.items)-1)
    if c == curses.KEY_RIGHT:
      item =  self.current_item()
      if item.childMenu is None:
        return
      self.stack.append((self.items,self.position))
      self.items = self.get_item(self.position).childMenu
      self.position = 0
    if c == curses.KEY_LEFT:
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
    if menuItem.childMenu is not None and p == self.position:
      s = s + ' >'
    return self.clip(s)

  def text1(self):
    return self.text_at(self.position)

  def text2(self):
    return self.text_at(self.position+1)

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
  stdscr.nodelay(1)
  while not menu.quit():
    win.addstr(1,1,menu.text1())
    win.addstr(2,1,menu.text2())
    win.refresh()
    c = stdscr.getch()
    if c!=-1:
      menu.process_key(c)
    time.sleep(0.01)
      
finally:
  curses.nocbreak()
  stdscr.keypad(0)
  curses.echo()
  curses.endwin()
  curses.curs_set(1)

