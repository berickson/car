import sys,tty,termios
import datetime
import time
import dateutil.parser
from select import select
from Car import Car

recording_file_path = 'recording.csv'
recording = open(recording_file_path,'r');
record_start_time = None
playback_start_time = datetime.datetime.now()

car = Car()
car.set_rc_mode()
while True:
  s = recording.readline()
  if not s:
    break
  fields = s.split(',')
  line_time = dateutil.parser.parse(fields[0])
  if record_start_time == None:
    record_start_time = line_time
  
  t_now = datetime.datetime.now()
  t_wait = (line_time - record_start_time) - (t_now - playback_start_time)
  if t_wait.total_seconds() > 0:
    time.sleep(t_wait.total_seconds())
    
  car.set_speed_and_steering(fields[5], fields[3])

car.set_manual_mode() 
print 'all done'

