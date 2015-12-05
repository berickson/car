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
while car.dynamics.reading_count == 0:
  time.sleep(0.01) 

start_heading = car.dynamics.heading


while True:
  s = recording.readline()
  if not s:
    break
  fields = s.split(',')
  dyn.set_from_log(fields)
  if record_start_time == None:
    record_start_time = dyn.datetime
    record_start_heading = dyn.heading
  
  t_now = datetime.datetime.now()
  t_wait = (dyn.datetime - record_start_time) - (t_now - playback_start_time)
  if t_wait.total_seconds() > 0:
    time.sleep(t_wait.total_seconds())
  
  # adjust steering based on heading error
  actual_turn = car.dynamics.heading - start_heading
  expected_turn = dyn.heading - record_start_heading
  original_steer_angle = car.angle_for_steering(dyn.str)
  steer_angle = original_steer_angle + expected_turn - actual_turn
  str = car.steering_for_angle(steer_angle)
    
  car.set_speed_and_steering(dyn.esc, str)

car.set_manual_mode() 
print 'all done'

