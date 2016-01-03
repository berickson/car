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
import Queue

max_velocity = 5.
automatic = True


def clamp(value, min_value, max_value):
  if value < min_value:
    return min_value
  if value > max_value:
    return max_value
  return value

def play_route():

  max_steering_degrees_per_second = 250.
  max_steering_angle = 30.
  last_steering_angle = 0.
  steering_angle = 0.
  last_ms = None


#  route = Route()
#  route.add_node(1,-.35,2)
#  route.add_node(2,-.35,10)
#  route.add_node(3,-.35,1)
#  route.add_node(4,-.35,0.5)
#  route.add_node(5,-1.35,0.5)
#  route.add_node(6,-.35,0.5)
#  route.add_node(5,.5,0.5)
#  route.add_node(4,-.35,0.5)
  route = Route()
  route.load_from_file('recordings/recording_041.csv.path', velocity = 1.0)


  
  car = Car()
  queue = Queue.Queue()
  
  try:
    if automatic: car.set_rc_mode()
    car.add_listener(queue)
    message = queue.get(block=True, timeout = 0.5)
    last_ms = message.ms

    # keep going until we run out of track  
    while True:
      message = queue.get(block=True, timeout = 0.5)
      elapsed_sec = (message.ms - last_ms) / 1000.
      (x,y) = car.front_position()
      cte = route.cross_track_error(x,y)
      if cte is None:
        break;

      # calculate speed 
      velocity = route.velocity()
      if velocity is None:
        velocity = max_velocity
      if car.velocity < velocity:
        esc_ms = car.min_forward_speed + 3
      else:
        esc_ms = car.min_forward_speed - 10
        
      # calculate steering
      segment_heading = degrees(route.heading_radians())
      car_heading = car.heading_degrees()
      desired_steering_angle = standardized_degrees(segment_heading - car_heading - 40. * cte)
      max_delta = elapsed_sec * max_steering_degrees_per_second;
      steering_delta = clamp(desired_steering_angle - last_steering_angle,-max_delta,max_delta)
      steering_angle = clamp(steering_angle + steering_delta, -max_steering_angle, max_steering_angle)
      
      
      str_ms = car.steering_for_angle(steering_angle)
   
      print("i: {:.2f} xg: {:.2f} gy:{:.2f} x: {:.2f} y:{:.2f} cte:{:.2f} v:{:.2f} heading:{:.2f} segment_heading: {:.2f} steering_degrees: {:.2f}".format(
         route.index,
         route.nodes[route.index+1].x,
         route.nodes[route.index+1].y,         
         x,
         y,
         cte,
         car.velocity,
         car_heading, 
         segment_heading,
         steering_angle))
     
      
      # send to car
      if automatic: car.set_speed_and_steering(esc_ms, str_ms)
      
      # remember state for next loop
      last_ms = message.ms
      last_steering_angle = steering_angle
      
  finally:
    car.set_speed_and_steering(1500,1500)
    car.set_manual_mode()
    car.remove_listener(queue)
    
  
if __name__ == '__main__':
  play_route()
  
