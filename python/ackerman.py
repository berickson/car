#!/usr/local/bin/python
# coding: utf-8
          
from math import *
from geometry import *

class Ackerman :
  def __init__(self, front_wheelbase_width, wheelbase_length, x=0.0, y=0.0, heading_radians=0.0):
    self.w = front_wheelbase_width
    self.l = wheelbase_length
    self.x = x
    self.y = y
    self.heading = heading_radians
    
  def __repr__(self):
    return 'Ackerman x: {:.5} y: {:.5} heading: {:.5}°'.format(
      self.x,
      self.y,
      degrees(self.heading))

  def move_left_wheel(self, outside_wheel_angle, wheel_distance, debug = False):
  
    # avoid errors for very small angles
    if abs(outside_wheel_angle) < 0.00001:
      outside_wheel_angle = 0.
      
    if debug: print '\noutside_wheel_angle: {:.3} wheel_distance: {:.3}'.format(
      degrees(outside_wheel_angle), 
      wheel_distance)

    if outside_wheel_angle == 0.:
      turn_angle = 0.
      arc_distance = wheel_distance
      forward = arc_distance
      left = 0.
    else:
      # calculate front wheel horizontal offset from center of rear wheels to left wheel
      offset = -self.w/2. if outside_wheel_angle < 0.  else self.w/2.
            
      # calculate turn radius of center rear of car
      #r_car = abs(self.l / tan(outside_wheel_angle) - offset)
      r_car = self.l / tan(outside_wheel_angle) - offset
       
      # calculate turn radius of left wheel
      r_left = sqrt((r_car + offset)**2 + self.l**2)
      # make radius negative for right turns
      if outside_wheel_angle < 0:
        r_left = -r_left
      if debug: print 'r_car: {:.4} r_left: {:.4}'.format(r_car, r_left)
      
      # calculate angle travelled
      turn_angle = wheel_distance / (2.*r_left)
      arc_distance = r_car * turn_angle * 2.
      forward = r_car * sin(turn_angle) * 2.
      left = r_car * (1.-cos(turn_angle)) * 2.

    if debug: print 'turn_angle: {:.4} arc_distance: {:.4} forward: {:.4} left: {:.4}'.format(turn_angle, arc_distance, forward, left)
    
    # finally, move the car based on left and forward
    self.x += cos(self.heading)*forward
    self.y += sin(self.heading)*forward
    self.x += sin(self.heading)*left
    self.y += cos(self.heading)*left
    self.heading = standardized_radians(self.heading + turn_angle)
    
    if debug: print str(self)

if __name__ == '__main__':

  angles = [0., 0.0001, 1., 45., 90.]
  for outside_angle_degrees in angles:
    outside_angle = radians(outside_angle_degrees)
    car = Ackerman(front_wheelbase_width = 10, wheelbase_length = 20)
    car.move_left_wheel(outside_angle, 10., debug = True)
    car.move_left_wheel(-outside_angle, 10., debug = True)

