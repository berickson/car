#!/usr/bin/env python2.7
# coding: utf-8

import time
import sys
from car import Car
car = Car()

sleep_time = 5.0
print("waiting {} seconds for things to settle\n".format(sleep_time))
time.sleep(sleep_time)


neutral_speed = 1500
neutral_steer = 1500

increment = 10
wait_seconds = 2.0
move_threshold = 0.5



def does_it_move(speed):
  car.set_rc_mode()
  car.set_esc_and_str(speed,neutral_steer)
  current_test_start_time = time.time()
  moved = False
  start_inches = car.ping_inches();
  while time.time() - current_test_start_time < wait_seconds and not moved:
    car.set_esc_and_str(speed,neutral_steer)
    time.sleep(0.05)
    new_inches = car.ping_inches()
    delta = abs(new_inches - start_inches)
    sys.stdout.write("esc: {}  {:5.3} inches, delta is {:5.3} inches\r".format(
      speed,
      new_inches, 
      delta))
    sys.stdout.flush()
    if delta > move_threshold:
      moved = True
      print
  car.set_esc_and_str(neutral_speed,neutral_steer)
  car.set_manual_mode()
  return moved



def calibrate_min_forward_esc():
  speed = 1540
  moved = False
  while not moved:
    moved = does_it_move(speed)
    if not moved:
      speed+=3
  time.sleep(3) # allow to stop completely
  return speed
  
def calibrate_min_reverse_esc():
  time.sleep(3)

  speed = 1460
  moved = False
  while not moved:
    moved = does_it_move(speed)
    if not moved:
      speed-=3
  return speed


min_forward_esc = calibrate_min_forward_esc()
min_reverse_esc = calibrate_min_reverse_esc()

print 'min_forward_esc:  {}  min_reverse_esc: {}'.format(min_forward_esc, min_reverse_esc)

