import time
import dateutil.parser
import threading


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
      self.reading_count = self.reading_count + 1
    except IndexError as e:
      print fields
   

class Car:
  def __init__(self):
    print 'car init'
    print 'enabling dyanics output'
    self.write_command('td+')
    self.dynamics = Dynamics()
    self.ouput_thread = threading.Thread(target=self._monitor_output, args = ())
    self.ouput_thread.daemon = True
    self.ouput_thread.start()
    self.odometer = 0
    
    self.quit = False
    
  def __del__(self):
    print 'car delete'
    self.quit = True
    self.output_thread.join()
    
  def write_command(self, s):
    command = open('/dev/car/command','w')
    command.write("{0}\n".format(s))
    
  def _monitor_output(self):
    self.output = open('/var/log/car/output.log','r')
    self.output.seek(0,2) # go to end of file
    lines_printed = 0
    while not self.quit:
      s = self.output.readline()
      if s:
        fields = s.split(',')
        if len(fields) > 1:
          if fields[1] == 'TRACE_DYNAMICS':
            self.dynamics.set_from_log(fields)
            self.odometer += self.dynamics.rpm_ticks
      else:
        time.sleep(0.001)



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


