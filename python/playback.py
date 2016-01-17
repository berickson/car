#!/usr/bin/env python
import sys,tty,termios
import datetime
import time
import dateutil.parser
from select import select
from car import Car, Dynamics, degrees_diff
from recording import Recording

def playback():
  record_start_time = None
  playback_start_time = datetime.datetime.now()

  car = Car()
  car.set_rc_mode()

  start_heading = car.dynamics.heading
  recording = Recording()

  while recording.read():
    dyn = recording.current()

    if record_start_time == None:
      record_start_time = dyn.datetime
      record_start_heading = dyn.heading
    
    t_now = datetime.datetime.now()
    t_wait = (dyn.datetime - record_start_time) - (t_now - playback_start_time)
    if t_wait.total_seconds() > 0:
      time.sleep(t_wait.total_seconds())
    
    # adjust steering based on heading error
    actual_turn = degrees_diff(start_heading, car.dynamics.heading)
    expected_turn = degrees_diff(record_start_heading, dyn.heading)
    original_steer_angle = car.angle_for_steering(dyn.str)
    steer_angle = original_steer_angle + degrees_diff(actual_turn, expected_turn)
    str = car.steering_for_angle(steer_angle)
      
    car.set_esc_and_str(dyn.esc, str)

  car.set_manual_mode() 
  print 'all done'

if __name__ == '__main__':
  playback()
  
