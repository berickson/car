fl#!/usr/bin/env python2.7
# coding: utf-8

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


def clamp(value, min_value, max_value):
  if value < min_value:
    return min_value
  if value > max_value:
    return max_value
  return value

def esc_for_velocity(goal_velocity, car, is_reverse):
  # calculate speed 
  car_velocity = car.get_velocity_meters_per_second()
  error = car_velocity - goal_velocity
  
  # set limits for forward and reverse
  if goal_velocity is None:
    goal_velocity = max_velocity
  
  if is_reverse:
    goal_velocity = -abs(goal_velocity)
    error = car_velocity - goal_velocity
    speed_up_esc = car.min_reverse_esc - 80
    slow_down_esc = 1500#car.min_forward_esc + 10
    maintain_esc = car.min_reverse_esc
    
    if error > 0.: # too slow
      esc_ms = speed_up_esc
    elif error < 0.2:
      esc_ms = slow_down_esc
    else:
      esc_ms = maintain_esc

  else:
    if error > 0.2:
      esc_ms = 1500#car.min_reverse_esc # slow down esc
    elif error > 0.:
      esc_ms = car.esc_for_velocity(clamp(goal_velocity- 3.*error,0.1,999.))
    else:
      esc_ms = car.esc_for_velocity(goal_velocity  - error)
  #special case for goal velocity of zero, always use neutral
  if goal_velocity == 0.:
    esc_ms = 1500
  return esc_ms

def steering_angle_by_cte(car, route):  
  cte = route.cross_track_error()

  # calculate steering
  segment_heading = degrees(route.heading_radians())
  car_heading = car.heading_degrees()

  # fix headings by opposite steering if going reverse
  heading_fix = segment_heading - car_heading
  cte_fix = -80 * cte
  if car.get_velocity_meters_per_second() < 0: #route.is_reverse():
    heading_fix = -heading_fix
    cte_fix = -2.*cte_fix # amplify fix for reverse
    
  steering_angle = standardized_degrees(heading_fix + cte_fix)
  return steering_angle

# steer to point t seconds + d meters ahead, use cte for reverse
def steering_angle_by_look_ahead(car,route,d=0.05,t=0.1):
  v = car.get_velocity_meters_per_second()
  if v < 0:
    return steering_angle_by_cte(car,route)
  (x,y) = route.get_position_ahead(d+v*t)
  car_x,car_y = car.front_position()
  heading_radians = car.heading_radians()
  desired_radians = math.atan2(y-car_Y,x-car_x)
  return degrees(desire_radians-heading_radians)


def drift(print_progress = True,car=None):
  # Start with a highly curved  route
  route = Route()
  
  circles = 0.25
  r = 1. # meters for circle
  theta = 0.
  steps = 360.
  while theta < circles*2*pi:
    route.add_node(r*sin(theta),r-r*cos(theta))
    theta += (2*pi)/steps
  
  route.optimize_velocity(max_velocity = 99., max_acceleration = 99.)
  print repr(route) 
  
 
  
  last_ms = None
  
  #print route 

  
  if car is None:
    car = Car()
  queue = Queue.Queue()
  
  try:
    car.set_rc_mode()
    car.add_listener(queue)
    message = queue.get(block=True, timeout = 0.5)
    #print repr(message)
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
      (x,y) = car.front_position()
      (rear_x,rear_y) = car.rear_position()
      route.set_position(x,y,rear_x,rear_y,car_velocity)
       
      #steering_angle = steering_angle_by_cte(car,route)
      steering_angle = steering_angle_by_look_ahead(car,route)
      
      str_ms = car.steering_for_angle(steering_angle)
   
      drift_angle_degrees = standardized_degrees(segment_heading-car.heading_degrees())
      if drift_angle_degrees > 30.:
        esc_ms = esc_for_velocity(1.,car,False)
      else:
        esc_ms = esc_for_velocity(99.,car,False)
      #esc_ms = esc_for_velocity(route.velocity(), car, route.is_reverse())
      print 'heading',car.heading_degrees(),'seg',segment_heading,'drift angle',drift_angle_degrees
      if route.done():
        esc_ms = esc_for_velocity(0.,car,False)
        
      if print_progress and False:
        print("t: {:.1f} i: {} xg: {:.2f} gy:{:.2f} gv: {:.2f}  v:{:.2f} x: {:.2f} y:{:.2f} reverse: {} cte:{:.2f} heading:{:.2f} segment_heading: {:.2f} steering_degrees: {:.2f} esc:{}".format(
           time.time() - start_time,
           route.index,
           route.nodes[route.index+1].x,
           route.nodes[route.index+1].y,         
           route.velocity(),
           car.get_velocity_meters_per_second(),
           x,
           y,
           route.is_reverse(),
           cte,
           car_heading, 
           segment_heading,
           steering_angle,
           esc_ms))
     
      
      # send to car
      car.set_esc_and_str(esc_ms, str_ms)
      
      
      if route.done() and car_velocity == 0:
        break;
      
  finally:
    car.set_esc_and_str(1500,1500)
    car.set_manual_mode()
    car.remove_listener(queue)  

  # Have outside front wheel following the desired route (alternatively use front center of car)
  #Apply 100% speed
  #Detect drift with these conditions:
  #Heading change no longer follows Ackerman model
  #Rear wheels  spinning
  #While drifting:
  #Update position as a function of steering angle, heading from imu, delta heading and delta wheel clicks (or velocity) of front wheel.
  #Using the new car heading, calculate how outside front wheel will need to point to stay on track. Turn to that angle.
  #If rear wheel is going at unsafe speed, decrease throttle
  #If the front outside wheel can't turn to the outside enough to stay on the route, decrease throttle to decrease slip. This happens because the rear of the car slipped too far forward compared to the front.
  #Try to maintain a car angle so that the front wheel still is about 10-20 degrees from its maximum steering angle.
  #If front wheel can't turn to the outside enough, because it reached maximum steering angle, decrease throttle to create less slip so the rear of the car can swing back.
  #If car angle is getting too close to path angle, increase throttle.
  #If getting close to goal, stop drifting


def play_route(route, car = None, print_progress = False):
  last_ms = None
  
  #print route 


  if car is None:
    car = Car()
  queue = Queue.Queue()
  
  try:
    car.set_rc_mode()
    car.add_listener(queue)
    message = queue.get(block=True, timeout = 0.5)
    #print repr(message)
    last_ms = message.ms
    start_time = time.time()

    # keep going until we run out of track  
    car.lcd.display_text('press any key\nto abort')
    while car.lcd.getch() is None:
      try:
        message = queue.get(block=True, timeout = 0.5)
      except:
        print 'message timed out at: '+datetime.datetime.now().strftime("%H:%M:%S.%f")
        print 'last message received:' + repr(message)
        print 
        raise
      (x,y) = car.front_position()
      (rear_x,rear_y) = car.rear_position()
      car_velocity = car.get_velocity_meters_per_second()
      route.set_position(x,y,rear_x,rear_y,car_velocity)
       
      cte = route.cross_track_error()

      # calculate steering
      segment_heading = degrees(route.heading_radians())
      car_heading = car.heading_degrees()

      # fix headings by opposite steering if going reverse
      heading_fix = segment_heading - car_heading
      cte_fix = -80 * cte
      if car_velocity < 0: #route.is_reverse():
        heading_fix = -heading_fix
        cte_fix = -2.*cte_fix # amplify fix for reverse
        
      steering_angle = standardized_degrees(heading_fix + cte_fix)
      
      str_ms = car.steering_for_angle(steering_angle)
   
   
      esc_ms = esc_for_velocity(route.velocity(), car, route.is_reverse())
      
        
      if print_progress:
        print("t: {:.1f} i: {} xg: {:.2f} gy:{:.2f} gv: {:.2f}  v:{:.2f} x: {:.2f} y:{:.2f} reverse: {} cte:{:.2f} heading:{:.2f} segment_heading: {:.2f} steering_degrees: {:.2f} esc:{}".format(
           time.time() - start_time,
           route.index,
           route.nodes[route.index+1].x,
           route.nodes[route.index+1].y,         
           route.velocity(),
           car_velocity,
           x,
           y,
           route.is_reverse(),
           cte,
           car_heading, 
           segment_heading,
           steering_angle,
           esc_ms))
     
      
      # send to car
      car.set_esc_and_str(esc_ms, str_ms)
      
      
      if route.done() and car_velocity == 0:
        break;
      
  finally:
    car.set_esc_and_str(1500,1500)
    car.set_manual_mode()
    car.remove_listener(queue)
    


def play_route_main():  

  import argparse
  parser = argparse.ArgumentParser(description = 'Route follower')
  parser.add_argument(
    'input_path',
    nargs='?',
    default='',
    help='path file to play, defaults to latest')
  parser.add_argument(
    '-a','--max_a',
    nargs='?',
    type=float,
    default=1.0,
    help='max_acceleration')
  parser.add_argument(
    '-d','--drift',
    action='store_true',
    help='drift mode, may override other variables')
    
  parser.add_argument(
    '-t', '--trace',
    action='store_true',

    help='turn on tracing')
  parser.add_argument(
    '-v','--max_v',
    nargs='?',
    type=float,
    default='1.0',
    help='max_velocity')
  args = parser.parse_args()
  max_a = args.max_a
  max_v = args.max_v
  
  if args.drift:
    drift()
  else:
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
  
    play_route(route, print_progress = args.trace)
  
if __name__ == '__main__':
  play_route()
