#!/usr/bin/env python2.7

import time
import serial
import sys
import datetime
import time
import glob
import os
from select import select

def write_logs(l):
  log_file = open('/var/log/car/')
  

def get_commands(command_file):
  while True:
    l = command_file.readline().strip()
    if(l != ''):
      print('got command{0}'.format(l))
      yield(l)
    else:
      return
      
def get_output(s):
  if(s.inWaiting() > 0):
    while(s.inWaiting() > 0):
      l = s.readline()
      ts = datetime.datetime.now()
      yield "{0}\t{1}".format(ts,l)
      
def run(command_file):
  while True:
    for usb_path in glob.glob('/dev/ttyACM*'):
      try:
        s = serial.Serial(usb_path)
        while True:
          did_work = False
          for c in get_commands(command_file):
            s.write('{0}\n'.format(c))
            did_work = True
          for o in get_output(s):
            write_logs(o)
            did_work = True
          if did_work == False:
            time.sleep(0.001)
      except IOError:
          time.sleep(1.0)
print 'about to open commands'
command_file = os.open('/dev/car/command',os.O_RDONLY | os.O_NONBLOCK)
print 'commands file opened'
run(command_file)

