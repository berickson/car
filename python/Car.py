import time
import dateutil.parser
import threading

#returns theta2-theta1 in range of [-180,180)
def angle_diff(theta1, theta2):
  return (theta2 - theta1  + 180 )% 360 - 180

class Dynamics:
  def __init__(self):
    self.reading_count = 0
  def set_from_log(self,fields):
    try:
      self.datetime = dateutil.parser.parse(fields[0])
      self.str = int(fields[3])
      self.esc = int(fields[5])
      self.ax = int(fields[7])
      self.ay = int(fields[8])
      self.az = int(fields[9])
      self.heading = float(fields[11])
      self.rpm_pps_raw = int(fields[13])
      self.rpm_pps = int(fields[14])
      self.rpm_ticks = int(fields[15])
      self.odometer = int(fields[16])
      self.ping_inches = float(fields[18])
      self.reading_count = self.reading_count + 1
    except (IndexError, ValueError) as e:
      pass


class Car:
  def __init__(self):
    print 'car init'
    print 'enabling dynamics output'
    self.quit = False
    self.write_command('td+')
    self.dynamics = Dynamics()
    self.output_thread = threading.Thread(target=self._monitor_output, args = ())
    self.output_thread.daemon = True
    self.output_thread.start()
    
    self.min_forward_speed = 1545
    self.min_reverse_speed = 1445

    
    
    
  def __del__(self):
    print 'car delete'
    self.quit = True
    self.output_thread.join()
    
  def write_command(self, s):
    command = open('/dev/car/command','w')
    print 'Sending command "{0}"'.format(s)
    command.write("{0}\n".format(s))
    
  def _monitor_output(self):
    self.output = open('/var/log/car/output.log','r')
    self.output.seek(0,2) # go to end of file
    lines_printed = 0
    while not self.quit:
      s = self.output.readline()
      if s:
        fields = s.split(',')
        if fields != None:
          if len(fields) > 1:
            if fields[1] == 'TRACE_DYNAMICS':
              self.dynamics.set_from_log(fields)
      else:
        time.sleep(0.001)
        
  def set_rc_mode(self):
    self.write_command('rc')

  def set_manual_mode(self):
    self.write_command('m')
    
  def set_speed_and_steering(self, speed, steering):
     self.write_command('pse {0},{1}'.format(steering, speed))
     
  def reverse(self, ticks, goal_heading = None):
    print('going reverse {0} ticks'.format(ticks))
    
    center = 1450
    
    goal_odometer = self.dynamics.odometer - ticks
    if goal_heading == None:
      goal_heading = self.dynamics.heading
    steering = center
    self.set_rc_mode()
    
    while self.dynamics.odometer > goal_odometer:
      speed = self.min_reverse_speed - 3
    
      # adjust steering
      heading_error = angle_diff(goal_heading, self.dynamics.heading)
      print('current ticks {0}  goal ticks {1} heading_error {2} rpm_pps {3}'.format(self.dynamics.odometer, goal_odometer, heading_error, self.dynamics.rpm_pps))
      steering = center +heading_error * 5
      
      # adjust speed
      if self.dynamics.rpm_pps <- 350:
        speed = self.min_reverse_speed
      if self.dynamics.rpm_pps <- 400:
        speed = self.min_reverse_speed+2
      
       
      self.set_speed_and_steering(speed, steering)
      time.sleep(.02)
    print('forward mode complete')
    self.set_speed_and_steering(1500,center)
    self.set_manual_mode()
 
  def forward(self, ticks, goal_heading = None):
    print('going forward {0} ticks'.format(ticks))
    
    center = 1450
    
    goal_odometer = self.dynamics.odometer + ticks
    if goal_heading == None:
      goal_heading = self.dynamics.heading
    steering = center
    self.set_rc_mode()
    
    while self.dynamics.odometer < goal_odometer:
      speed = self.min_forward_speed + 5
    
      # adjust steering
      heading_error = angle_diff(goal_heading, self.dynamics.heading)
      print('current ticks {0}  goal ticks {1} heading_error {2} rpm_pps {3}'.format(self.dynamics.odometer, goal_odometer, heading_error, self.dynamics.rpm_pps))
      steering = center - heading_error * 5
     
      # adjust speed
      if self.dynamics.rpm_pps > 350:
        speed = self.min_forward_speed
      
       
      self.set_speed_and_steering(speed, steering)
      time.sleep(.02)
    print('forward mode complete')
    self.set_speed_and_steering(1500,center)
    self.set_manual_mode()


def main():
  print 'testing car'
  car = Car()
  print ('waiting for first reading')
  while car.dynamics.reading_count == 0:
    time.sleep(0.01) 
  
  print ('waiting for steering to go below 1200')
  while car.dynamics.str > 1200:
    print 'odometer: {0}'.format(car.odometer)
    time.sleep(0.25) 
  print ('it happened!')
  

if __name__ == "__main__":
  main()


