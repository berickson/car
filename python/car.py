#!/usr/bin/env python2.7
# coding: utf-8

import time
import dateutil.parser
import threading
import ConfigParser
from ackerman import Ackerman
from math import *
from geometry import *
import pprint




class Dynamics:
  def __init__(self):
    self.reading_count = 0
  def __repr__(self):
    return pprint.pformat(vars(self),depth=1)
      
      
  def set_from_log(self,fields):
    self.datetime = dateutil.parser.parse(fields[0])
    self.str = int(fields[3])
    self.esc = int(fields[5])
    self.ax = float(fields[7])
    self.ay = float(fields[8])
    self.az = float(fields[9])
    self.spur_delta_us = int(fields[11])
    self.spur_last_us = int(fields[12])
    self.spur_odo = int(fields[14])
    self.ping_millimeters = int(fields[16])
    self.odometer_front_left =  int(fields[18])
    self.odometer_ticks = self.odometer_front_left
    self.odometer_front_left_last_us =  int(fields[19])
    self.odometer_last_us = self.odometer_front_left_last_us
    self.odometer_front_right = int(fields[21])
    self.odometer_front_right_last_us = int(fields[22])
    self.odometer_back_left = int(fields[24])
    self.odometer_back_left_last_us = int(fields[25])
    self.odometer_back_right = int(fields[27])
    self.odometer_back_right_last_us = int(fields[28])
    self.ms = int(fields[30])
    self.us = int(fields[32])
    self.yaw = float(fields[34])
    self.heading = self.yaw
    self.pitch = float(fields[35])
    self.roll = float(fields[36])
    self.battery_voltage = float(fields[38])
    self.reading_count = self.reading_count + 1


class Car:
  def __init__(self, online = True):
    self.config_path = 'car.ini'
    self.read_configuration()
    self.online = online
    self.reset_odometry()
    self.listener = None
    self.last_verified_velocity = 0.0
    self.lcd = None
    self.usb_error_count = 0
    
    if self.online:
      from lcd import Lcd
    
      self.lcd = Lcd()
      self.quit = False
      self.write_command('td+') # first command normally fails, so write a blank command
      self.write_command('td+')
      self.output_thread = threading.Thread(target=self._monitor_output, args = ())
      self.output_thread.daemon = True
      self.output_thread.start()
      while self.dynamics.reading_count == 0:
        time.sleep(0.01) 

  def display_text(self, s):
    if self.lcd is not None:
      self.lcd.display_text(s)

  def reset_odometry(self):
    self.reading_count = 0
    self.dynamics = Dynamics()
    self.velocity = 0.0
    self.last_velocity = 0.0
    self.heading_adjustment = 0.
    self.odometer_start = 0
    self.ackerman = Ackerman(
      front_wheelbase_width = self.front_wheelbase_width_in_meters, 
      wheelbase_length = self.wheelbase_length_in_meters)

  def read_configuration(self):
  
    # odometry
    self.meters_per_odometer_tick = float(self.get_option('calibration','meters_per_odometer_tick'))
    self.gyro_adjustment_factor = float(self.get_option('calibration','gyro_adjustment_factor'))
    
    # esc and steering
    self.center_steering_us = int(self.get_option('calibration','center_steering_us'))
    self.min_forward_esc = int(self.get_option('calibration','min_forward_esc'))
    self.min_reverse_esc = int(self.get_option('calibration','min_reverse_esc'))
    self.reverse_center_steering_us = int(self.get_option('calibration','reverse_center_steering_us'))
    
    # car dimensions
    self.front_wheelbase_width_in_meters = float(self.get_option('calibration','front_wheelbase_width_in_meters'))
    self.rear_wheelbase_width_in_meters = float(self.get_option('calibration','rear_wheelbase_width_in_meters'))
    self.wheelbase_length_in_meters = float(self.get_option('calibration','wheelbase_length_in_meters'))
    
    # infer general dimensions
    # todo: put these in config to get complete shape of car
    self.length = self.wheelbase_length_in_meters
    self.width = self.front_wheelbase_width_in_meters

  def add_listener(self, listener):
    if self.listener is not None:
      raise Exception("only one listener allowed")
      
    self.listener = listener    

  def remove_listener(self, listener):
    if self.listener is not listener:
     raise Exception("Unknown listener")
     
    self.listener = None

  def __exit__(self, type_unused, value_unused, traceback_unused):
    self.quit = True
    if self.online:
      self.output_thread.join()

  def get_option(self,section,option):
    config = ConfigParser.ConfigParser()
    with open(self.config_path, 'rwb+') as configfile:
      config.readfp(configfile)
    return config.get(section, option)
        
  def set_option(self,section,option,value):
    config = ConfigParser.ConfigParser()
    with open(self.config_path, 'rwb+') as configfile:
        config.readfp(configfile)
        
    config.set(section,option,value)

    with open(self.config_path, 'rwb+') as configfile:
        config.write(configfile)
        
   
  def write_command(self, s):
    if not self.online:
      raise Exception("must be online")
    with open('/dev/car','w') as command:
      #print 'Sending command "{0}"'.format(s)
      command.write("{0}\n".format(s))
    
  def _monitor_output(self):
    self.output = open('/var/log/car','r')
    self.output.seek(0,2) # go to end of file
    line_start = None
    while not self.quit:
      s = self.output.readline()
      if s:
        if line_start is not None:
          s = line_start + s
          line_start = None
        if not s.endswith('\n'):
          line_start = s
          continue
          
        self.process_line_from_log(s)              
      else:
        time.sleep(0.001)

  def process_line_from_log(self, s):
    # sanity check.  Only process valid TRACE_DYNAMICS log lines
    fields = s.split(',')
    if fields == None:
      return
    if len(fields) < 10:
      return
    if fields[1] != 'TD':
      return
    if len(fields) != 39:
      self.usb_error_count = self.usb_error_count + 1
      #print 'invalid TD packet with {} fields: {}'.format(len(fields),s)
      return
      
    # todo, handle contention with different threads
    # for now, make changeover quick and don't mess
    # with existing structures, maybe by keeping a big list
    current = Dynamics()
    try:
      current.set_from_log(fields)
    except:
      return
    previous = self.dynamics
    self.dynamics = current
    self.reading_count += 1
    if self.reading_count > 1:
      self.apply_dynamics(current, previous)
    else:
      self.original_dynamics = current
      
    if self.listener != None:
      self.listener.put(current)
  
  def get_usb_error_count(self):
    return self.usb_error_count
    
  # returns ping distance in meters
  def ping_inches(self):
    return self.ping_meters()/0.0254

  def ping_distance(self):
    return self.ping_meters()

  def ping_meters(self):
    return self.dynamics.ping_millimeters / 1000.
  
  
  def battery_voltage(self):
    return self.dynamics.battery_voltage
  
  def apply_dynamics(self, current, previous):
    # correct heading with adjustment factor
    self.heading_adjustment += (1. - self.gyro_adjustment_factor) * standardized_degrees(current.heading - previous.heading)
   
    # if wheels have moved, update ackerman
    wheel_distance_meters = (current.odometer_ticks-previous.odometer_ticks)  * self.meters_per_odometer_tick
    if abs(wheel_distance_meters) > 0.:
      outside_wheel_angle = radians(self.angle_for_steering(previous.str))
      self.ackerman.move_left_wheel(outside_wheel_angle, wheel_distance_meters, self.heading_radians())
      
    # update velocity
    if current.odometer_last_us != previous.odometer_last_us:
      elapsed_seconds = (current.odometer_last_us - previous.odometer_last_us) / 1000000.
      self.velocity = wheel_distance_meters / elapsed_seconds
      self.last_verified_velocity = self.velocity
    else:
      # no tick this time, how long has it been?
      seconds_since_tick = ( current.us - current.odometer_last_us ) / 1000000.
      if seconds_since_tick > 0.1:
        # it's been a long time, let's call velocity zero
        self.velocity = 0.0
      else:
        # we've had a tick recently, fastest possible speed is when a tick is about to happen
        # let's use smaller of that and previously certain velocity
        max_possible = self.meters_per_odometer_tick / seconds_since_tick
        if max_possible > abs(self.last_verified_velocity):
          self.velocity = self.last_verified_velocity
        else:
          if self.last_verified_velocity > 0:
            self.velocity = max_possible
          else:
            self.velocity = -max_possible

    #print("x:{:.2f} y:{:.2f} heading:{:.2f}".format(self.ackerman.x, self.ackerman.y, relative_heading))
  
  # returns the velocity in meters/sec
  # if the car is reversing, velocity will be negative
  def get_velocity_meters_per_second(self):
    return self.velocity;
  
  # returns position of rear of car (between two rear wheels), this starts at -wheelbase_length_in_meters,0
  def rear_position(self):
    return (self.ackerman.x - self.wheelbase_length_in_meters, self.ackerman.y)
  
  # returns position of front of car (between two front wheels), this starts at 0,0
  def front_position(self):
    h = radians(self.heading_degrees());
    l = self.wheelbase_length_in_meters
    return (self.ackerman.x + l * cos(h) - l , self.ackerman.y + l * sin(h))
    
  def heading_degrees(self):
    return standardized_degrees(self.dynamics.heading - self.original_dynamics.heading + self.heading_adjustment)
    
  def heading_radians(self):  
    return radians(self.heading_degrees())
    
  
  def set_rc_mode(self):
    self.write_command('rc')

  def set_manual_mode(self):
    self.write_command('m')
  
  def wheels_angle(self):
    return self.angle_for_steering(self.dynamics.str) 
    
  
  def esc_for_velocity(self, v):
    data = [
      (-2., 1200), # made up numbers for negative v: todo: find empirically
      (-1., 1250),
      (0.0,  1500),
      (0.1, 1645),
      (0.34, 1659),
      (0.85, 1679),
      (1.2, 1699),
      (1.71, 1719),
      (1.88, 1739),
      (2.22, 1759),
      (2.6, 1779),
      (3.0, 1799),
      (14.0, 2000)
      ]
    return table_lookup(data, v)
      
  def angle_for_steering(self, str):
    data = [
      (30,1000),
      (25,1104),
      (20,1189),
      (15,1235),
      (10,1268),
      (5, 1390),
      (0, 1450),
      (-5, 1528),
      (-10, 1607),
      (-15,1688),
      (-20, 1723),
      (-25, 1768),
      (-30, 1858)]
    last = len(data)-1
    if str <= data[0][1]:
      return data[0][0]
    if str >= data[last][1]:
      return data[last][0]
    for i in range(0,last):
      if str <= data[i+1][1]:
        return interpolate(
          str, data[i][1], data[i][0], data[i+1][1], data[i+1][0])
  
  # returns the steering pulse for give steering angle
  # of the outside wheel
  def steering_for_angle(self, theta):
    data = [
      (30,1000),
      (25,1104),
      (20,1189),
      (15,1235),
      (10,1268),
      (5, 1390),
      (0, 1450),
      (-5, 1528),
      (-10, 1607),
      (-15,1688),
      (-20, 1723),
      (-25, 1768),
      (-30, 1858)]
    last = len(data)-1
    if theta >= data[0][0]:
      return data[0][1]
    if theta <= data[last][0]:
      return data[last][1]
    for i in range(0,last):
      if theta >= data[i+1][0]:
        return interpolate(
          theta, data[i][0], data[i][1], data[i+1][0], data[i+1][1])
     
    
  def set_esc_and_str(self, speed, steering):
     self.write_command('pse {0},{1}'.format(int(steering), int(speed)))
  
  def zero_odometer(self):
    self.odometer_start = self.dynamics.odometer_ticks

  def odometer_meters(self):
    return (self.dynamics.odometer_ticks - self.odometer_start) * self.meters_per_odometer_tick
  
  def odometer_front_left(self):
    return self.dynamics.odometer_front_left
  def odometer_front_right(self):
    return self.dynamics.odometer_front_right
  def odometer_back_left(self):
    return self.dynamics.odometer_back_left
  def odometer_back_right(self):
    return self.dynamics.odometer_back_right
 
  # returns where you should steer to if you wish to go to goal_heading
  def steering_for_goal_heading_degrees(self, goal_heading, reverse = False):
    direction = -1 if reverse else 1
    heading_error = degrees_diff(goal_heading, self.heading_degrees())
    steering = self.steering_for_angle(-direction * heading_error)
    return steering
  
  def is_drifting(self):
    return self.ackerman.is_drifting()

  def forward(self, meters, goal_heading = None, fixed_steering_us = None, max_speed = 2.0):
    ticks = int(meters/self.meters_per_odometer_tick)
    if fixed_steering_us != None:
      steering = fixed_steering_us
    
    if goal_heading == None:
      goal_heading = self.dynamics.heading
    
    #use a direction of 1 / -1 to help with the math for forward / reverse
    if ticks > 0:
      direction = 1
      min_esc = 1500
      max_esc = self.esc_for_velocity(max_speed)
    else:
      direction = -1
      min_esc = 1500
      max_esc = self.esc_for_velocity(-max_speed) 
    
    goal_odometer = self.dynamics.odometer_ticks + ticks

    self.set_rc_mode()
    while self.dynamics.odometer_ticks * direction < goal_odometer * direction:
      esc = max_esc
    
      # adjust steering if fixed steering wasn't selected
      if fixed_steering_us == None:
        heading_error = degrees_diff(goal_heading, self.dynamics.heading)
        steering = self.steering_for_angle(-direction * heading_error)
   
      # adjust speed
      if abs(self.get_velocity_meters_per_second()) > max_speed:
        esc = min_esc
             
      self.set_esc_and_str(esc, steering)
      time.sleep(.02)
    self.set_esc_and_str(1500,steering)
    slowdown_start = time.time()
    while (time.time()-slowdown_start < 3):
      if fixed_steering_us == None:
        heading_error = degrees_diff(goal_heading, self.dynamics.heading)
        steering = self.steering_for_angle(-direction * heading_error * 1.5)
      time.sleep(0.01)


class FakeCar(Car):
    def __init__(self, recording_file_path):
        self.recording_file_path = recording_file_path;
        Car.__init__(self, online=False)
        self.reset()
        self.done = False
        self.step()
        
    def reset(self):
        self.dynamics_file = open(self.recording_file_path)
        self.reset_odometry()
        self.step()
    
    def step(self):
        s = self.dynamics_file.readline()
        if not s:
          return False
        self.process_line_from_log(s)
        return True

      


def main():
  count = 0
  print 'testing car'
  car = Car()
  # wait here so messages can come from thread and we can trap ctrl-c
  while True:
    count = count + 1
    time.sleep(0.01) 
#    if car.is_drifting():
#      print '*'
#    else:
#      print '.'

    s = "drifting: {} heading: {:5.2f} v: {:5.2f}  rear:({:5.2f},{:5.2f}) front:({:5.2f},{:5.2f}) ".format(
      car.is_drifting(),
      car.heading_degrees(),
      car.get_velocity_meters_per_second(),
      *car.rear_position()+
      car.front_position()
      )
    #if count%20 == 0:
    #  car.display_text("h:{:.1f}\nf:{:5.2f},{:5.2f}".format(car.heading_degrees(),*car.front_position()))
  

if __name__ == "__main__":
  main()


