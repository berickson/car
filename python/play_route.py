import sys,tty,termios
import datetime
import time
import dateutil.parser
from select import select
from car import Car, Dynamics, degrees_diff
from recording import Recording
from route import *
from math import *

desired_velocity = 0.5

def play_route():
  route = Route()
  route.add_node(0.,0)
  route.add_node(1.,-0.25)
  route.add_node(2.,-0.25)
  
#  route.add_node(.5,-0.25)
#  route.add_node(.8,-0.1)
#  route.add_node(1.3,1.)

  
  car = Car()
  car.set_rc_mode()

  # keep going until we run out of track  
  while True:
    (x,y) = car.position()
    cte = route.cross_track_error(x,y)
    if cte is None:
      break;

    # calculate speed 
    if car.velocity < desired_velocity:
      esc_ms = car.min_forward_speed
    else:
      esc_ms = 1500
      
    # calculate steering
    segment_heading = degrees(route.heading_radians())
    car_heading = car.heading_degrees()
    steering_angle = -(segment_heading - car_heading + 30. * cte)
    
    str_ms = car.steering_for_angle(steering_angle)
 
    print("x: {:.2f} y:{:.2f} cte:{:.2f} v:{:.2f} heading:{:.2f} segment_heading: {:.2f} steer_degrees: {:.2f}".format(
       x,
       y,
       cte,
       car.velocity,
       car_heading, 
       segment_heading,
       steering_angle))
   
    
    # send to car
    car.set_speed_and_steering(esc_ms, str_ms)
    
    time.sleep(0.02)

  car.set_manual_mode()
    
  
if __name__ == '__main__':
  play_route()
  
