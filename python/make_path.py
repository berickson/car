from car import *
from recording import Recording
from ackerman import *
from filenames import *

def write_path_from_recording_file(inpath = None, outpath = None):
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
  heading_adjustment = 0.0
  
  if outpath == None:
    outpath = inpath + '.path'
  
  
  # print header row
  with open(outpath,'w') as outfile:
    header = ",".join(["secs","x","y","heading","adj","esc","str","m/s"])
    outfile.write(header+"\n")
    while(f.read()):
      current = next
      next = f.current()
      #heading_adjustment -= .01 * standardized_degrees(next.heading - current.heading)
      wheel_ticks = next.odometer_ticks - current.odometer_ticks
      distance = wheel_ticks * car.meters_per_odometer_tick
      ackerman.heading = radians(standardized_degrees(current.heading + heading_adjustment - start_heading))
      ackerman.move_left_wheel(
        outside_wheel_angle  = radians(car.angle_for_steering(current.str)), 
        wheel_distance = distance)
      

      wheel_meters_per_second = distance / ((next.ms - current.ms) / 1000.)
      seconds = (current.ms - start_ms)/1000.

      line =  ",".join([
         str(seconds),
         str(ackerman.x), 
         str(ackerman.y), 
         str(standardized_degrees(current.heading - start_heading)),
         str(heading_adjustment),
         str(current.esc),
         str(current.str),
         str(wheel_meters_per_second)])
      outfile.write(line+"\n")
       
if __name__ == '__main__':
  input_path = latest_filename('recordings','recording','csv')
  write_path_from_recording_file(input_path)
       

