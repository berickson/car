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

import vision.capture

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
config.track_name = 'desk'
config.route_name = FileNames().get_route_names(config.track_name)[0]
config.max_a = 0.1
config.max_v = 1.0
config.t_ahead = 0.2
config.d_ahead = 0.05
config.k_smooth = 0.4


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

def t_ahead(t = None):
  if t is not None:
    config.t_ahead = t
  return config.t_ahead

def d_ahead(d = None):
  if d is not None:
    config.d_ahead = d
  return config.d_ahead

def k_smooth(k = None):
  if k is not None:
    config.k_smooth = k
  return config.k_smooth
  
def track_name(n = None):
  if n is not None:
    config.track_name = n
    config.route_name = ""
    update_route_menu()
  return config.track_name

def route_name(n = None):
  if n is not None:
    config.route_name = n
    update_route_menu()
  return config.route_name


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

def restart():
  car.display_text("restart svc")
  time.sleep(0.25)
  car.lcd.set_backlight(0)
  os.system('sudo service car restart')
  
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
  MenuItem('shutdown',action=shutdown),
  MenuItem('restart svc',action=restart)
  ]


def record():
  car.reset_odometry()
  f = FileNames()
  track_name = config.track_name
  route_name = f.next_route_name(track_name)
  os.makedirs(f.get_route_folder(track_name, route_name))
  
  recording_file_path = f.recording_file_path(track_name, route_name)
  make_recording(car = car, recording_file_path = recording_file_path )
  car.lcd.display_text("making path")
  path_file_path = f.path_file_path(track_name, route_name)
  write_path_from_recording_file(inpath=recording_file_path,outpath=path_file_path)
  config.route_name = route_name
  update_route_menu()

def go():
    f = FileNames()
    config.run_name = f.next_run_name(config.track_name, config.route_name)
    run_folder = f.get_run_folder(config.track_name, config.route_name, config.run_name)
    os.makedirs(run_folder)
    
    car.reset_odometry()
    input_path = f.path_file_path(config.track_name,config.route_name)
    stereo_video_paths = f.stereo_video_file_paths(config.track_name,config.route_name,config.run_name)
    capture = vision.capture.Capture(stereo_video_paths=stereo_video_paths)
    capture.begin()

    rte = route.Route()
    car.display_text('loading route')
    rte.load_from_file(input_path)
    rte.smooth(k_smooth=config.k_smooth)
    rte.optimize_velocity(max_velocity = config.max_v, max_acceleration = config.max_a)
    car.display_text('playing route')
    play_route(rte, car, k_smooth = config.k_smooth, d_ahead = config.d_ahead, t_ahead = config.t_ahead)
    capture.end()

acceleration_menu = selection_menu(max_a, np.arange(0.25,10.1,0.25))
velocity_menu = selection_menu(max_v,np.arange(0.5,20.1,0.5))
k_smooth_menu = selection_menu(k_smooth,np.arange(0.,1.1,0.1))
t_ahead_menu = selection_menu(t_ahead,np.arange(0.,1.1,0.1))
d_ahead_menu = selection_menu(d_ahead,np.arange(0.,1.001,0.01))
track_selection_menu = selection_menu(track_name, FileNames().get_track_names())
def route_selection_menu():
  return selection_menu(route_name, FileNames().get_route_names(config.track_name))

def update_route_menu():  
  global route_menu
  route_menu = [
    MenuItem(lambda:'track[{}]'.format(config.track_name),sub_menu=track_selection_menu),
    MenuItem(lambda:'route[{}]'.format(config.route_name),sub_menu=route_selection_menu()),
    MenuItem('go',action=go),
    MenuItem('record',action=record),
    MenuItem(lambda:'max_a[{}]'.format(config.max_a),sub_menu = acceleration_menu ),
    MenuItem(lambda:'max_v[{}]'.format(config.max_v),sub_menu = velocity_menu),
    MenuItem(lambda:'k_smooth[{}]'.format(config.k_smooth),sub_menu = k_smooth_menu),
    MenuItem(lambda:'t_ahead[{}]'.format(config.t_ahead),sub_menu = t_ahead_menu),
    MenuItem(lambda:'d_ahead[{}]'.format(config.d_ahead),sub_menu = d_ahead_menu)
    ]
update_route_menu()

main_menu = [
    MenuItem('route',sub_menu=route_menu),
    MenuItem('pi',sub_menu = pi_menu),
##    MenuItem('vars',sub_menu = vars_menu)
    ]


vars_menu = [
    MenuItem(lambda:'{0:3.1f}v{1},{2}'.format(
      car.battery_voltage(),
      fixed_float_string(car.front_position()[0],4),
      fixed_float_string(car.front_position()[1],4)
      ),sub_menu = main_menu),
    MenuItem(lambda:ip_address(),sub_menu = main_menu),
  MenuItem(lambda:'usb_errors: {0}'.format(car.get_usb_error_count()),sub_menu = main_menu),
  MenuItem(lambda:'frnt:{0},{1}'.format(fixed_float_string(car.front_position()[0],4),
      fixed_float_string(car.front_position()[1],4)),sub_menu = main_menu),
  MenuItem(lambda:'rear:{0},{1}'.format(fixed_float_string(car.rear_position()[0],4),
      fixed_float_string(car.rear_position()[1],4)),sub_menu = main_menu),
  MenuItem(lambda:'head: {0}'.format(fixed_float_string(car.heading_degrees(),4)),sub_menu = main_menu),
  MenuItem(lambda:'odo_fl: {0}'.format(car.odometer_front_left()),sub_menu = main_menu),
  MenuItem(lambda:'odo_fr: {0}'.format(car.odometer_front_right()),sub_menu = main_menu),
  MenuItem(lambda:'odo_bl: {0}'.format(car.odometer_back_left()),sub_menu = main_menu),
  MenuItem(lambda:'odo_br: {0}'.format(car.odometer_back_right()),sub_menu = main_menu)
  ]


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
  menu = Menu(vars_menu)
  menu.run()
  del car

if __name__ == "__main__":
  main()

