#!/usr/bin/env python2.7
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
    self.drifting = False
    
  def __repr__(self):
    return 'Ackerman x: {:.5} y: {:.5} heading: {:.5}°'.format(
      self.x,
      self.y,
      degrees(self.heading))
      
  def is_drifting(self):
    return self.drifting
  
  
  # x is ahead, l is to left
  def arc_to_relative_location(self,x,y):
    x = float(x)
    y = float(y)
    l = self.l
    if abs(y) > 0.000001:
      z=sqrt((l+x/2)**2+(x/y*(l+x/2))**2)
      c=sqrt(x**2+y**2)
      r=sqrt(z**2+(c**2)/2)
      steer_radians = asin(l/r)
      arc_radians = 2*asin((c/2)/z)
      arc_len = r * arc_radians
    else:
      steer_radians = 0.0
      arc_radians = 0.0
      r = 1.0E100
      arc_len = distance(0.0,0.0,x,y)
      

    if y < 0.:
      steer_radians = -steer_radians
    
    return (steer_radians,arc_radians,r, arc_len)
    

  def move_left_wheel(self, outside_wheel_angle, wheel_distance, new_heading = None, debug = False):
  
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
    
    computed_new_heading = standardized_radians(self.heading + turn_angle)
    if new_heading is None:
      self.heading = computed_new_heading
    else:
      delta_heading_error = radians_diff(new_heading,computed_new_heading)
      if delta_heading_error > 0.005:
        self.drifting = True
      else:
        self.drifting = False
      self.heading = new_heading
    
    if debug: print str(self)

# calculates an arc where the front wheel will travel to
# point x ahead and point y t left
def test_arc_to_relative_location(l,x,y):
  car = Ackerman(front_wheelbase_width = 10, wheelbase_length = l)
  (steer_radians,arc_radians,r,arc_len) =  car.arc_to_relative_location(x,y)
  print "l:",l, " x: ", x, " y:", y, "steer degrees:", degrees(steer_radians), "arc degrees:", degrees(arc_radians), "turn radius:", r, "arc length:",arc_len
  
  

def arc_to_relative_location_tests():
  test_arc_to_relative_location(l=20,x=20,y=20)
  test_arc_to_relative_location(l=20,x=40,y=20)
  test_arc_to_relative_location(l=20,x=20,y=0.01)
  test_arc_to_relative_location(l=20,x=0,y=20)
  test_arc_to_relative_location(l=20,x=-1,y=20)


  test_arc_to_relative_location(l=20,x=20,y=20)

  test_arc_to_relative_location(l=20,x=20,y=-20)
  test_arc_to_relative_location(l=20,x=40,y=-20)
  test_arc_to_relative_location(l=20,x=20,y=-0.01)

  test_arc_to_relative_location(l=20,x=20,y=0)


def test_other():
  angles = [0., 0.0001, 1., 45., 90.]
  for outside_angle_degrees in angles:
    outside_angle = radians(outside_angle_degrees)
    car = Ackerman(front_wheelbase_width = 10, wheelbase_length = 20)
    car.move_left_wheel(outside_angle, 10., debug = True)
    car.move_left_wheel(-outside_angle, 10., debug = True)

if __name__ == '__main__':
  arc_to_relative_location_tests()


