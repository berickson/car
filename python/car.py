import time
import dateutil.parser
import threading
import ConfigParser
from ackerman import Ackerman
from math import *
from geometry import *



#returns y for given x based on x1,y1,x2,y2
def interpolate(x, x1, y1, x2, y2):
  x = float(x)
  x1 = float(x1)
  x2 = float(x2)
  y1 = float(y1)
  y2 = float(y2)
  
  #print("x:{} x1:{} y1:{} x2:{} y2:{}".format(x,x1,y1,x2,y2))
  m = (y2 - y1)/( x2 - x1 )
  y = y1 + m * (x-x1)
  return y



class Dynamics:
  def __init__(self):
    self.reading_count = 0
  def set_from_log(self,fields):
#    try:
      self.datetime = dateutil.parser.parse(fields[0])
      self.str = int(fields[3])
      self.esc = int(fields[5])
      self.ax = float(fields[7])
      self.ay = float(fields[8])
      self.az = float(fields[9])
      self.heading = float(fields[11])
      self.rpm_pps_raw = int(fields[13])
      self.rpm_pps = int(fields[14])
      self.rpm_ticks = int(fields[15])
      self.engine_odometer = int(fields[16])
      self.ping_inches = float(fields[18])
      self.odometer_ticks = int(fields[20])
      self.ms = int(fields[22])
      self.us = int(fields[24])
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
    self.min_forward_speed = int(self.get_option('calibration','min_forward_speed'))
    self.min_reverse_speed = int(self.get_option('calibration','min_reverse_speed'))
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
    command = open('/dev/car/command','w')
    #print 'Sending command "{0}"'.format(s)
    command.write("{0}\n".format(s))
    
  def _monitor_output(self):
    self.output = open('/var/log/car/output.log','r')
    self.output.seek(0,2) # go to end of file
    lines_printed = 0
    while not self.quit:
      s = self.output.readline()
      if s:
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
      print 'invalid TRACE_DYNAMICS packet'
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
    self.velocity = wheel_distance / elapsed_time
    #print("x:{:.2f} y:{:.2f} heading:{:.2f}".format(self.ackerman.x, self.ackerman.y, relative_heading))
  
  
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
    
    
  
  def set_rc_mode(self):
    self.write_command('rc')

  def set_manual_mode(self):
    self.write_command('m')
  
  def wheels_angle(self):
    return self.angle_for_steering(self.dynamics.str) 
      
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
     
    
  def set_speed_and_steering(self, speed, steering):
     self.write_command('pse {0},{1}'.format(int(steering), int(speed)))

 
  def forward(self, meters, goal_heading = None, fixed_steering_us = None):
    ticks = int(meters/self.meters_per_odometer_tick)
    if fixed_steering_us != None:
      steering = fixed_steering_us
    
    if goal_heading == None:
      goal_heading = self.dynamics.heading
    
    #use a direction of 1 / -1 to help with the math for forward / reverse
    if ticks > 0:
      direction = 1
      min_speed = self.min_forward_speed - 3
      max_speed = self.min_forward_speed + 5
    else:
      direction = -1
      min_speed = self.min_reverse_speed + 1
      max_speed = self.min_reverse_speed - 3
    
    goal_odometer = self.dynamics.odometer_ticks + ticks

    self.set_rc_mode()
    while self.dynamics.odometer_ticks * direction < goal_odometer * direction:
      speed = max_speed
    
      # adjust steering if fixed steering wasn't selected
      if fixed_steering_us == None:
        heading_error = degrees_diff(goal_heading, self.dynamics.heading)
        steering = self.steering_for_angle(-direction * heading_error)
   
      # adjust speed
      if abs(self.dynamics.rpm_pps) > 350:
        speed = min_speed
             
      self.set_speed_and_steering(speed, steering)
      time.sleep(.02)
    self.set_speed_and_steering(1500,steering)
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


