#!/usr/bin/env python
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
from filenames import *
import Queue

max_velocity = 5.
automatic = True



def clamp(value, min_value, max_value):
  if value < min_value:
    return min_value
  if value > max_value:
    return max_value
  return value

def around_bar():
  
  route = Route()
  route.load_from_file('recordings/recording_044.csv.path', velocity = 1.)
  return route

def around_kitchen():
  
  route = Route()
  route.load_from_file('recordings/recording_049.csv.path', velocity = 1.)
  return route

def hall_and_back():
  route = Route()
  route.load_from_file('recordings/recording_041.csv.path', velocity = 1.)
  return route
  

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
    print repr(message)
    last_ms = message.ms
    start_time = time.time()

    # keep going until we run out of track  
    while True:
      try:
        message = queue.get(block=True, timeout = 0.5)
      except:
        print 'message timed out at: '+datetime.datetime.now().strftime("%H:%M:%S.%f")
        print 'last message received:' + repr(message)
        print 
        raise
      elapsed_sec = (message.ms - last_ms) / 1000.
      (x,y) = car.front_position()
      (rear_x,rear_y) = car.rear_position()
      car_velocity = car.get_velocity_meters_per_second()
      route.set_position(x,y,rear_x,rear_y,car_velocity)
       
      cte = route.cross_track_error()

      # calculate speed 
      velocity = route.velocity()
      error = car_velocity - velocity
      
      # set limits for forward and reverse
      if velocity is None:
        velocity = max_velocity
      
      if route.is_reverse():
        error = velocity - car_velocity
        speed_up_esc = car.min_reverse_esc - 80
        slow_down_esc = car.min_forward_esc + 10
        maintain_esc = car.min_reverse_esc
        
        if error < 0.: # too slow
          esc_ms = speed_up_esc
        elif error > 0.2:
          esc_ms = slow_down_esc
        else:
          esc_ms = maintain_esc

      else:
        if error > 0.2:
          esc_ms = car.min_reverse_esc # slow down esc
        elif error > 0.:
          esc_ms = car.esc_for_velocity(clamp(velocity- 3.*error,0.1,999.))
        else:
          esc_ms = car.esc_for_velocity(velocity  - error)

#        speed_up_esc = car.esc_for_velocity(velocity + 1)
#        slow_down_esc = car.min_reverse_esc
#        maintain_esc = car.esc_for_velocity(velocity) #car.min_forward_esc
      
        
      # calculate steering
      segment_heading = degrees(route.heading_radians())
      car_heading = car.heading_degrees()

      # fix headings by opposite steering if going reverse
      heading_fix = segment_heading - car_heading
      cte_fix = -40 * cte
      if route.is_reverse():
        heading_fix = -heading_fix
        cte_fix = -10.*cte_fix # 10x cte fix for reverse!
        
      desired_steering_angle = standardized_degrees(heading_fix + cte_fix)
      
      max_delta = elapsed_sec * max_steering_degrees_per_second;
      steering_delta = clamp(desired_steering_angle - last_steering_angle,-max_delta,max_delta)
      steering_angle = clamp(steering_angle + steering_delta, -max_steering_angle, max_steering_angle)
      
      
      str_ms = car.steering_for_angle(steering_angle)
   
      print("t: {:.1f} i: {} xg: {:.2f} gy:{:.2f} gv: {:.2f} x: {:.2f} y:{:.2f} reverse: {} cte:{:.2f} v:{:.2f} heading:{:.2f} segment_heading: {:.2f} steering_degrees: {:.2f} esc:{}".format(
         time.time() - start_time,
         route.index,
         route.nodes[route.index+1].x,
         route.nodes[route.index+1].y,         
         velocity,
         x,
         y,
         route.is_reverse(),
         cte,
         car_velocity,
         car_heading, 
         segment_heading,
         steering_angle,
         esc_ms))
     
      
      # send to car
      if automatic: car.set_esc_and_str(esc_ms, str_ms)
      
      # remember state for next loop
      last_ms = message.ms
      last_steering_angle = steering_angle
      
      if route.done() and car_velocity == 0:
        break;
      
  finally:
    car.set_esc_and_str(1500,1500)
    car.set_manual_mode()
    car.remove_listener(queue)
    
  
if __name__ == '__main__':

  import argparse
  parser = argparse.ArgumentParser(description = 'Route follower')
  parser.add_argument(
    'input_path',
    nargs='?',
    default='',
    help='path file to play, defaults to latest')
  parser.add_argument(
    '--max_a',
    nargs='?',
    type=float,
    default=1.0,
    help='max_acceleration')
  parser.add_argument(
    '--max_v',
    nargs='?',
    type=float,
    default='1.0',
    help='max_velocity')
  args = parser.parse_args()
  max_a = args.max_a
  max_v = args.max_v
  
  input_path = args.input_path
  if input_path == "":
    input_path = latest_filename('recordings','recording','csv.path')
  route = Route()
  print 'loading route at', input_path
  route.load_from_file(input_path)

#  route = around_bar()
#  route = around_kitchen()
  print 'optimizing speeds along the route'
  route.optimize_velocity(max_velocity = max_v, max_acceleration = max_a)
  print route
  print 'playing route now, press ctrl-c to abort'
  
  play_route(route)
  
