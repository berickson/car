import sys,tty,termios
import datetime
import time
import dateutil.parser
import filenames
from select import select


folder = 'recordings'
prefix = 'recording'
suffix = '.csv'
recording_file_path = filenames.next_filename(folder=folder,prefix=prefix,suffix=suffix)

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
        recording.write(s);
  else:
    time.sleep(0.01)

recording.close()

print 'all done'

