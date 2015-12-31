from car import *
from recording import Recording
from ackerman import *
from filenames import *

def write_path_from_recording_file(inpath = None, outpath = None):
  car = FakeCar(recording_file_path = inpath)
  if outpath == None:
    outpath = inpath + '.path'
  
  
  with open(outpath,'w') as outfile:
    # print header row
    header = ",".join(["secs","x","y","heading","adj","esc","str","m/s"])
    outfile.write(header+"\n")

#    start_heading = car.heading_degrees()
    next = car.dynamics
    start = car.dynamics
   
    while(car.step()):
      current = next
      next = car.dynamics
      
      wheel_ticks = next.odometer_ticks - current.odometer_ticks
      distance = wheel_ticks * car.meters_per_odometer_tick
      wheel_meters_per_second = distance / ((next.ms - current.ms) / 1000.)
      
      seconds = (current.ms - start.ms)/1000.
      (x,y) = car.front_position()

      line =  ",".join([
         str(seconds),
         str(x), 
         str(y), 
 #        str(standardized_degrees(current.heading() - start_heading)),
         str(car.heading_degrees()),
         str(car.heading_adjustment),
         str(current.esc),
         str(current.str),
         str(wheel_meters_per_second)])
      outfile.write(line+"\n")
       
if __name__ == '__main__':
  input_path = latest_filename('recordings','recording','csv')
  write_path_from_recording_file(input_path)
       

