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
#    try:
      self.datetime = dateutil.parser.parse(fields[0])
      self.str = int(fields[3])
      self.esc = int(fields[5])
      self.ax = float(fields[7])
      self.ay = float(fields[8])
      self.az = float(fields[9])
#      self.heading = float(fields[11])
      self.spur_delta_us = int(fields[11])
      self.spur_last_us = int(fields[12])
      self.spur_odo = int(fields[14])
#      self.engine_odometer = int(fields[1])
      self.ping_millimeters = int(fields[16])
      self.odometer_ticks = int(fields[18])
      self.ms = int(fields[20])
      self.us = int(fields[22])
      self.yaw = float(fields[24])
      self.pitch = float(fields[25])
      self.roll = float(fields[26])
      self.reading_count = self.reading_count + 1
 #   except (IndexError, ValueError) as e:
#      pass


class Car:
  def __init__(self, online = True):
    self.config_path = 'car.ini'
    self.read_configuration()
    self.online = online
    self.reset_odometry()
    self.listener = None
    
    if self.online:
      self.quit = False
      self.write_command('td+') # first command normally fails, so write a blank command
      self.write_command('td+')
      self.output_thread = threading.Thread(target=self._monitor_output, args = ())
      self.output_thread.daemon = True
      self.output_thread.start()
      while self.dynamics.reading_count == 0:
        time.sleep(0.01) 


  def reset_odometry(self):
    self.reading_count = 0
    self.dynamics = Dynamics()
    self.velocity = 0.0
    self.last_velocity = 0.0
    self.heading_adjustment = 0.
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
    with open('/dev/car/command','w') as command:
      #print 'Sending command "{0}"'.format(s)
      command.write("{0}\n".format(s))
    
  def _monitor_output(self):
    self.output = open('/var/log/car/output.log','r')
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
    if fields[1] != 'TRACE_DYNAMICS':
      return
    if len(fields) != 29:
      print 'invalid TRACE_DYNAMICS packet: {}'.format(s)
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
  
  # returns ping distance in meters
  def ping_distance(self):
    return self.dynamics.ping_inches * 0.0254
  
  def apply_dynamics(self, current, previous):
    self.heading_adjustment += (1. - self.gyro_adjustment_factor) * standardized_degrees(current.heading - previous.heading)
    relative_heading = self.heading_degrees()
    relative_heading_radians = radians(relative_heading)
    outside_wheel_angle = radians(self.angle_for_steering(previous.str))
    wheel_distance = (current.odometer_ticks-previous.odometer_ticks)  * self.meters_per_odometer_tick
    self.ackerman.heading = relative_heading_radians
    self.ackerman.move_left_wheel(outside_wheel_angle, wheel_distance)
    elapsed_time = ( current.ms - previous.ms ) / 1000.
    
    self.last_velocity = self.velocity
    self.velocity = wheel_distance / elapsed_time
    #print("x:{:.2f} y:{:.2f} heading:{:.2f}".format(self.ackerman.x, self.ackerman.y, relative_heading))
  
  def get_velocity_meters_per_second(self):
    if self.last_velocity is not None:
      return (self.velocity + self.last_velocity) /2.
    elif self.velocity is not None:
      return self.velocity
    else:
      return 0.
  
  # returns position of rear of car (between two rear wheels), this starts at -wheelbase_length_in_meters,0
  def rear_position(self):
    return (self.ackerman.x, self.ackerman.y)
  
  # returns position of front of car (between two front wheels), this starts at 0,0
  def front_position(self):
    h = radians(self.heading_degrees());
    l = self.wheelbase_length_in_meters
    return (self.ackerman.x + l * cos(h) - l , self.ackerman.y + l * sin(h))
    
  def heading_degrees(self):
    return standardized_degrees(self.dynamics.heading - self.original_dynamics.heading + self.heading_adjustment)
    
    
  
  def set_rc_mode(self):
    self.write_command('rc')

  def set_manual_mode(self):
    self.write_command('m')
  
  def wheels_angle(self):
    return self.angle_for_steering(self.dynamics.str) 
    
  
  def esc_for_velocity(self, v):
    data = [
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

 
  def forward(self, meters, goal_heading = None, fixed_steering_us = None, max_speed = 2.0):
    ticks = int(meters/self.meters_per_odometer_tick)
    if fixed_steering_us != None:
      steering = fixed_steering_us
    
    if goal_heading == None:
      goal_heading = self.dynamics.heading
    
    #use a direction of 1 / -1 to help with the math for forward / reverse
    if ticks > 0:
      direction = 1
      min_esc = self.min_forward_esc - 3
      max_esc = self.min_forward_esc + 30
    else:
      direction = -1
      min_esc = self.min_reverse_esc + 1
      max_esc = self.min_reverse_esc - 30
    
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
  print 'testing car'
  car = Car()
  # wait here so messages can come from thread and we can trap ctrl-c
  while True:
    time.sleep(0.01) 
  

if __name__ == "__main__":
  main()


