#!/usr/bin/env python2.7
# coding: utf-8
import time
from geometry import *


def esc_for_power(power):
  if abs(power) < 0.01:
    return 1500
  if power < 0:
    return 1400 + power*400
  if power > 0:
    return 1600 + power*400
  


def main():
  import car
  car = car.Car()
  #power = raw_input('enter speed:')
  car.set_rc_mode()
  car.zero_odometer()
#  timer = Interval(interval=0.02)
  kp = 0.002
  kd = 0.002
  ki = 0.01
  last_error = 0.0

  error = 1500
  d_error = 0.0
  i_error = 0.0
  last_t = time.time()
  start_time = time.time()

  try:
    while abs(error) > 10 or d_error > 100:
#      f = car.odometer_front_left()
      f = 1500
      b = car.odometer_back_left()
      error = f - b
      dt = time.time()-last_t
      last_t = time.time()
      
      d_error = (error - last_error)/dt
      i_error = i_error + error*dt
      if abs(d_error) > 100. or abs(error) < 10:
        i_error = 0.
      last_error = error
      
      power = kp*error + kd*d_error + ki * i_error
      power = clamp(power,-1.,1.)
      speed = esc_for_power(float(power))
      speed = clamp(speed,1250,1750)
      steer = car.steering_for_goal_heading_degrees(0.)
      car.set_esc_and_str(speed = speed, steering = steer)
      print 'E:', error, 'dE:', d_error, 'iE:',i_error,'p:', power, 'esc:',speed
      time.sleep(0.05)
  finally:
    car.set_manual_mode()
  print 'done in',time.time()-start_time,'seconds'
    
  

if __name__=='__main__':
  main()
