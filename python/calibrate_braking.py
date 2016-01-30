#!/usr/local/bin/python
# coding: utf-8

from car import Car
import time


total_track_length = 4.
stop_track_length = 2. # required distance to slow at the end, todo: calculate based on max_speed
run_track_length = total_track_length - stop_track_length

max_speed = 1.
esc_test = 1500


car = Car()
goal_heading = car.heading_degrees()


# accelerate in straight line until you get to desired speed
start_odometer = car.odometer
v = 0.0 # assume we start at zero speed
aborted = False
while v < max_speed 
  if car.odometer_meters() - start_meters > run_track_length:
    aborted = True
    abort_reason = "ran out of track"
    break;
  esc = car.esc_for_velocity(speed + 2)
  str = car.steering_for_heading_degrees(goal_heading)
  car.set_esc_and_str(esc,str)
  time.sleep(0.02)
  v = car.get_velocity_meters_per_second()

# set the esc to given braking speed

# go straight and record data until one of three things happens
#
# 1. You come to a full stop
# 2. Your speed stabilizes
# 3. You reach distance limit
# 4. You detect that you are skidding



# put on "safe level" of brakes until your speed reaches zero (or negative)
while v > 0:
  esc = 1400
  str = car.steering_for_heading_degrees(goal_heading)
  car.set_esc_and_str(esc,str)
  time.sleep(0.02)
  v = car.get_velocity_meters_per_second()

# set everything to neutral
car.set_manual_mode()

# save the results of braking to two files
# car dynamics in one file
# test settings in another file

# do a quick analysis of data and print results


# distance to stop from speed, distance left in track
# whether skidding occurred
# estimate of acceleration using linear fit
# estimate of acceleration using polynomial fit (TBD: model for this, probably adding k*v^2 term for wind resistance)
