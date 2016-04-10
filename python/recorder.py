#!/usr/bin/env python

import sys,tty,termios
import datetime
import time
import dateutil.parser
import filenames
from select import select

from car import Car
import Adafruit_CharLCD as LCD

folder = 'recordings'
prefix = 'recording'
suffix = '.csv'
recording_file_path = filenames.next_filename(folder=folder,prefix=prefix,suffix=suffix)

def write_command_to_car(s):
  command = open('/dev/car','w')
  command.write(s)

def getch():
  import sys, tty, termios
  fd = sys.stdin.fileno()
  old_settings = termios.tcgetattr(fd)
  try:
    tty.setraw(sys.stdin.fileno())
    ch = sys.stdin.read(1)
  finally:
    termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
  return ch

def kbhit():
  dr,dw,de = select([sys.stdin],[],[],0)
  return dr <> []

def make_recording(car = None):
  output = open('/var/log/car','r')
  output.seek(0,2) # go to end of file

  write_command_to_car('td+\n')


  recording = open(recording_file_path,'w')


  print('recording, press any key to stop')
  car.display_text("recording\n-> stop")
  if car is None:
    car = Car()
  while not car.lcd.is_pressed(LCD.RIGHT):#kbhit():
    if car.lcd.is_pressed(LCD.RIGHT):
      break
    s = output.readline()
    if s:
      fields = s.split(',')
      if len(fields) > 1:
        if fields[1] == 'TD':
          recording.write(s);
    else:
      time.sleep(0.01)

  recording.close()

  print 'all done'

if __name__=="__main__":
  make_recording()
