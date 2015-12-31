import sys,tty,termios
import datetime
import time
import dateutil.parser
from select import select
from car import Car, Dynamics, degrees_diff
from recording import Recording
from route import *
from math import *

automatic = True
desired_velocity = 0.5

def play_route():
  route = Route()
  route.add_node(0.,0.)
#  route.add_node(1,-.25)
#  route.add_node(2,0)
#  route.add_node(3,-0.25)
#  route.add_node(4,0)
  route.add_node(5,0)

  #route.add_node(1.,-0.25)
  #route.add_node(3.,-0.25)
  
#  route.add_node(.5,-0.25)
#  route.add_node(.8,-0.1)
#  route.add_node(1.3,1.)

  
  car = Car()
  try:
    if automatic: car.set_rc_mode()

    # keep going until we run out of track  
    while True:
      (x,y) = car.front_position()
      
      # positive cte means car is to the right of track
      # None means eof
      cte = route.cross_track_error(x,y)
      if cte is None:
        break;

      # calculate speed 
      if car.velocity < desired_velocity:
        esc_ms = car.min_forward_speed+3
      else:
        esc_ms = car.min_forward_speed - 10
        
      # calculate steering
      segment_heading = degrees(route.heading_radians())
      car_heading = car.heading_degrees()
      steering_angle = segment_heading - car_heading + 20. * cte
      
      str_ms = car.steering_for_angle(steering_angle)
   
      print("i: {:.2f} x: {:.2f} y:{:.2f} cte:{:.2f} v:{:.2f} heading:{:.2f} segment_heading: {:.2f} steer_degrees: {:.2f}".format(
         route.index,
         x,
         y,
         cte,
         car.velocity,
         car_heading, 
         segment_heading,
         steering_angle))
     
      
      # send to car
      if automatic: car.set_speed_and_steering(esc_ms, str_ms) 
      
      time.sleep(0.02)
  finally:
    car.set_manual_mode()
    
  
if __name__ == '__main__':
  play_route()
  
