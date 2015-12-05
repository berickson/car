import sys,tty,termios
import datetime
import time
import dateutil.parser
from select import select


recording_file_path = 'recording.csv'

def write_command_to_car(s):
  command = open('/dev/car/command','w')
  command.write(s)

def getch():
  import sys, tty, termios
  fd = sys.stdin.fileno()
  old_settings = termios.tcgetattr(fd)
  try:
    tty.setraw(sys.stdin.fileno())
    ch = sys.stdin.read(1)
  finally:
    termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
  return ch

def kbhit():
  dr,dw,de = select([sys.stdin],[],[],0)
  return dr <> []

output = open('/var/log/car/output.log','r')
output.seek(0,2) # go to end of file

write_command_to_car('td+\n')


recording = open(recording_file_path,'w')


print('recording, press any key to stop')
while not kbhit():
  s = output.readline()
  if s:
    fields = s.split(',')
    if len(fields) > 1:
      if fields[1] == 'TRACE_DYNAMICS':
#        print('str: {0} esc: {1}'.format(fields[3], fields[5]))
        recording.write(s);
  else:
    time.sleep(0.01)

recording.close()

print('stopped recording, press any key to play back')
getch()

print('playing back recording')


recording = open(recording_file_path,'r');
record_start_time = None
playback_start_time = datetime.datetime.now()
write_command_to_car('rc') # set to remote control mode
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

  pse_command = 'pse{0},{1}'.format(fields[3], fields[5])
  write_command_to_car(pse_command)

write_command_to_car("m") # return to manual mode
write_command_to_car('td-') # turn off loggin
print 'all done'

