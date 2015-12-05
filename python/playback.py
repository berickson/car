import sys,tty,termios
import datetime
import time
import dateutil.parser
from select import select
from Car import Car, Dynamics

recording_file_path = 'recording.csv'
recording = open(recording_file_path,'r');
record_start_time = None
playback_start_time = datetime.datetime.now()

car = Car()
dyn = Dynamics()
car.set_rc_mode()

while True:
  s = recording.readline()
  if not s:
    break
  fields = s.split(',')
  dyn.set_from_log(fields)
  if record_start_time == None:
    record_start_time = dyn.datetime
  
  t_now = datetime.datetime.now()
  t_wait = (dyn.datetime - record_start_time) - (t_now - playback_start_time)
  if t_wait.total_seconds() > 0:
    time.sleep(t_wait.total_seconds())
    
  car.set_speed_and_steering(dyn.esc, dyn.str)

car.set_manual_mode() 
print 'all done'

