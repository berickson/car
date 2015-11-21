class Car:

  def __init__(self):
    print 'car init'
    
  def __del__(self):
    print 'car delete'
    
  def write_command(self, s):
    command = open('/dev/car/command','w')
    command.write(s)
    
  def _monitor_output():
    output = open('/var/log/car/output.log','r')
    output.seek(0,2) # go to end of file
    s = output.readline()
    if s:
      fields = s.split(',')
      if len(fields) > 1:
        if fields[1] == 'TRACE_DYNAMICS':
          print('str: {0} esc: {1}'.format(fields[3], fields[5]))
//          recording.write(s);
    else:
      time.sleep(0.01)



def main():
  print 'testing car'
  car = Car()
  car.write_command("td+")
  car._monitor_output()
  

if __name__ == "__main__":
  main()


