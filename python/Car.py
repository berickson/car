import time
import dateutil.parser
import threading

#returns theta2-theta1 in range of [-180,180)
def angle_diff(theta1, theta2):
  return (theta2 - theta1  + 180 )% 360 - 180

#returns y for given x based on x1,y1,x2,y2
def interpolate(x, x1, y1, x2, y2):
  #print("x:{} x1:{} y1:{} x2:{} y2:{}".format(x,x1,y1,x2,y2))
  m = (y2 - y1)/( x2 - x1 )
  y = y1 + m * (x-x1)
  return y



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
    #print 'Sending command "{0}"'.format(s)
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
  
  # returns the steering pulse for give steering angle
  # of the outside wheel
  def steering_for_angle(self, theta):
    data = [
      (-30,1000),
      (-25,1104),
      (-20,1189),
      (-15,1235),
      (-10,1268),
      (-5, 1390),
      (0, 1450),
      (5, 1528),
      (10, 1607),
      (15,1688),
      (20, 1723),
      (25, 1768),
      (30, 1858)]
    last = len(data)-1
    if theta <= data[0][0]:
      return data[0][1]
    if theta >= data[last][0]:
      return data[last][1]
    for i in range(0,last):
      if theta <= data[i+1][0]:
        return interpolate(
          theta, data[i][0], data[i][1], data[i+1][0], data[i+1][1])
     
    
  def set_speed_and_steering(self, speed, steering):
     self.write_command('pse {0},{1}'.format(steering, speed))

 
  def forward(self, ticks, goal_heading = None):
    print('going forward {0} ticks'.format(ticks))
    
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
    
    goal_odometer = self.dynamics.odometer + ticks

    self.set_rc_mode()
    while self.dynamics.odometer * direction < goal_odometer * direction:
      speed = max_speed
    
      # adjust steering
      heading_error = angle_diff(goal_heading, self.dynamics.heading)
      steering = self.steering_for_angle(-direction * heading_error)
   
      # adjust speed
      if abs(self.dynamics.rpm_pps) > 350:
        speed = min_speed
             
      self.set_speed_and_steering(speed, steering)
      time.sleep(.02)
    
    print 'it should slow down here'
    self.set_speed_and_steering(1500,steering)
    slowdown_start = time.time()
    while (time.time()-slowdown_start < 3):
      heading_error = angle_diff(goal_heading, self.dynamics.heading)
      steering = self.steering_for_angle(-direction * heading_error)
      self.set_speed_and_steering(1500,steering)
      time.sleep(0.01)
      
    print('forward mode complete')
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


