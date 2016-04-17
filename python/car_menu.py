#!/usr/bin/env python2.7
# coding: utf-8

import time
import numpy as np
import pdb
import os
import route
from subprocess import Popen
from lcd import LCD, Lcd
from car import Car
from recorder import make_recording
from make_route import write_path_from_recording_file
from play_route import play_route
from filenames import *

shutdown_flag = False


# stackoverflow answer from http://stackoverflow.com/a/1267524/383967
def ip_address():
  import socket
  try:
    return [l for l in ([ip for ip in socket.gethostbyname_ex(socket.gethostname())[2] if not ip.startswith("127.")][:1], [[(s.connect(('8.8.8.8', 53)), s.getsockname()[0], s.close()) for s in [socket.socket(socket.AF_INET, socket.SOCK_DGRAM)]][0][1]]) if l][0][0]
  except:
    return "not connected"

class Config:
  pass
config = Config()
config.max_a = 0.1
config.max_v = 1.0


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

# reboot the computer
def reboot():
  #p = Popen(['sudo shutdown -r now /r']) # async
  car.display_text("rebooting")
  time.sleep(0.25)
  car.lcd.set_backlight(0)
  os.system('sudo shutdown -r now /r')

def shutdown():
  car.display_text("shutting down")
  time.sleep(0.25)
  car.lcd.set_backlight(0)
  os.system('sudo shutdown now /r')
  
sub1 = [MenuItem(lambda:time.strftime('%H:%M:%S'))];


# returns a float string fixed to n characters
def fixed_float_string(f,n):
  return "{:<.4f}".format(f).replace("0.",".")[:n]


def selection_menu(value_function, values):
  return  map(
    lambda a: MenuItem(
      lambda: selection_text(a,value_function()),
      action=lambda:value_function(a)),
    values)

pi_menu = [
  MenuItem('reboot',action=reboot),
  MenuItem('shutdown',action=shutdown)
  ]


def make_path():
  car.lcd.display_text("making path")
  write_path_from_recording_file()

def go():
    input_path = latest_filename('recordings','recording','csv.path')
    rte = route.Route()
    car.display_text('loading route')
    rte.load_from_file(input_path)
    rte.optimize_velocity(max_velocity = config.max_v, max_acceleration = config.max_a)
    car.display_text('playing route')
    play_route(rte, car)

acceleration_menu = selection_menu(max_a, np.arange(0.1,3,0.1))
velocity_menu = selection_menu(max_v,np.arange(0.5,20,0.5))

  
route_menu = [
  MenuItem('go',action=go),
  MenuItem('make path',action=make_path),
  MenuItem('record',action=lambda:make_recording(car = car)),
  MenuItem(lambda:'max_a[{}]'.format(config.max_a),sub_menu = acceleration_menu ),
  MenuItem(lambda:'max_v[{}]'.format(config.max_v),sub_menu = velocity_menu)
  ]

main_menu = [
    MenuItem(lambda:'{0:3.1f}v{1},{2}'.format(
      car.battery_voltage(),
      fixed_float_string(car.front_position()[0],4),
      fixed_float_string(car.front_position()[1],4)
      ),sub_menu = sub1),
    MenuItem(lambda:ip_address()),
    MenuItem('route',sub_menu=route_menu),
    MenuItem('pi',sub_menu = pi_menu)]


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
    if c == LCD.SELECT:
      pass # todo: home / display off
#      item =  self.current_item()

    if c == LCD.UP:
      self.position = self.position - 1
      self.position = max(self.position,0)
    if c == LCD.DOWN:
      self.position = self.position + 1
      self.position = min(self.position,len(self.items)-1)
    if c == LCD.RIGHT:
      item =  self.current_item()
      if item.sub_menu is not None:
        self.stack.append((self.items,self.position))
        self.items = self.get_item(self.position).sub_menu
        self.position = 0
      if item.action is not None:
        item.action()      
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
      s = s + '>'
    if menuItem.action is not None and p == self.position:
      s = s + '*'
    return self.clip(s)

  def text1(self):
    return self.text_at(self.position)

  def text2(self):
    return self.text_at(self.position+1)

  def quit(self):
    return self._quit or shutdown_flag
    
  def run(self):
    try:
      lcd = car.lcd
      while not self.quit():
        t = self.text1()+"\n"+self.text2()
        lcd.display_text(t)
        c = lcd.getch()
        if c is not None:
          self.process_key(c)
        time.sleep(0.03)
    finally:
      car.display_text("goodbye")
      print('goodbye')
      time.sleep(0.25)
      lcd.set_backlight(0)
      del lcd

def main():
  global car
  car = Car()
  menu = Menu(main_menu)
  menu.run()
  del car

if __name__ == "__main__":
  main()

