from Car import *
from Recording import Recording
from ackerman import *

def write_path_from_recording_file(inpath = None):
  odometer = 0
  car = Car(online=False)
  f = Recording(inpath)
  ackerman = Ackerman(
    front_wheelbase_width = car.front_wheelbase_width_in_meters, 
    wheelbase_length = car.wheelbase_length_in_meters)
  
  last_odometer_ticks = 0
  if not f.read():
    raise Exception("could not read file")
  current = f.current()
  start_heading = current.heading
  start_ms = current.ms
  next = current
  
  # print header row
  print ",".join(["secs","x","y","heading","esc","str","m/s"])
  while(f.read()):
    current = next
    next = f.current()
    wheel_ticks = next.odometer_ticks - current.odometer_ticks
    distance = wheel_ticks * car.meters_per_odometer_tick
    ackerman.heading = radians(degrees_diff(current.heading, start_heading))
    ackerman.move_left_wheel(
      outside_wheel_angle  = radians(car.angle_for_steering(current.str)), 
      wheel_distance = distance)
    

    wheel_meters_per_second = distance / ((next.ms - current.ms) / 1000.)
    seconds = (current.ms - start_ms)/1000.

    print ",".join([
       str(seconds),
       str(ackerman.x), 
       str(ackerman.y), 
       str(degrees_diff(current.heading, start_heading)),
       str(current.esc),
       str(current.str),
       str(wheel_meters_per_second)])
    print next.datetime
       
if __name__ == '__main__':
  write_path_from_recording_file('recordings/recording_015.csv')
