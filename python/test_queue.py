#!/usr/bin/env python
import sys,tty,termios
import datetime
import time
import dateutil.parser
from select import select
from car import Car, Dynamics, degrees_diff
from recording import Recording
from route import *
from math import *
from geometry import *
from filenames import *
import Queue


def test_queue():
  car = Car()
  queue = Queue.Queue()
  
  try:
    car.add_listener(queue)
    i = 0
    # keep going until we run out of track  
    while True:
      message = queue.get(block=True, timeout = 0.5)
      i = i + 1
      if i % 100 == 0:
        print i
  except:
    print i
    raise

  
if __name__ == '__main__':
  test_queue()

