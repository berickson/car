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
    print '*',
    return min_value
  if value > max_value:
    print '*',
    return max_value
  return value


def hall_and_back():
  route = Route()
  route.load_from_file('recordings/recording_041.csv.path', velocity = 1.)
  return route
  
def straight_route():
  x = 0.1
  while x <= 8.5 + 0.0001: #door is about 8.5 from end of toolbox by desk
    route.add_node(x,0.)
    x += 0.05
  route.optimize_velocity(max_velocity = 5., 
    max_acceleration = 1.0) # 1.0 - safe indoors (3 cm overshoot)
                            # 1.5 - agressive indoors (5 cm overshoot)
                            # 2.0 - very agressive indoors (10 cm overshoot)

def play_route(route):

  max_steering_degrees_per_second = 250.
  max_steering_angle = 30.
  last_steering_angle = 0.
  steering_angle = 0.
  last_ms = None
  
  print route 


  
  car = Car()
  queue = Queue.Queue()
  
  try:
    if automatic: car.set_rc_mode()
    car.add_listener(queue)
    message = queue.get(block=True, timeout = 0.5)
    last_ms = message.ms
    start_time = time.time()

    # keep going until we run out of track  
    while True:
      message = queue.get(block=True, timeout = 0.5)
      elapsed_sec = (message.ms - last_ms) / 1000.
      (x,y) = car.front_position()
      route.set_position(x,y)
       
      cte = route.cross_track_error()

      # calculate speed 
      velocity = route.velocity()
      if velocity is None:
        velocity = max_velocity
      if car.velocity < velocity:
        esc_ms = car.min_forward_speed + 3
      elif car.velocity > velocity + .2:
        esc_ms = 1400
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
   
      print("t: {:.1f} i: {:.2f} xg: {:.2f} gy:{:.2f} gv: {:.2f} x: {:.2f} y:{:.2f} cte:{:.2f} v:{:.2f} heading:{:.2f} segment_heading: {:.2f} steering_degrees: {:.2f} esc:{}".format(
         time.time() - start_time,
         route.index,
         route.nodes[route.index+1].x,
         route.nodes[route.index+1].y,         
         velocity,
         x,
         y,
         cte,
         car.velocity,
         car_heading, 
         segment_heading,
         steering_angle,
         esc_ms))
     
      
      # send to car
      if automatic: car.set_speed_and_steering(esc_ms, str_ms)
      
      # remember state for next loop
      last_ms = message.ms
      last_steering_angle = steering_angle
      
      if route.done() and car.velocity<=0:
        break;
      
  finally:
    car.set_speed_and_steering(1500,1500)
    car.set_manual_mode()
    car.remove_listener(queue)
    
  
if __name__ == '__main__':
  play_route(hall_and_back())
  
