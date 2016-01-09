#!/usr/bin/env python

from car import *
from recording import Recording
from ackerman import *
from filenames import *

def write_path_from_recording_file(inpath = None, outpath = None, min_length = 0.03):
  car = FakeCar(recording_file_path = inpath)
  if outpath == None:
    outpath = inpath + '.path'
  
  
  with open(outpath,'w') as outfile:
    # print header row
    header = ",".join(["secs","x","y","rear_x", "rear_y", "reverse", "heading","adj","esc","str","m/s"])
    outfile.write(header+"\n")

#    start_heading = car.heading_degrees()
    next = car.dynamics
    start = car.dynamics
    current = car.dynamics
    (x,y) = car.front_position()
    
    reverse = False
    next_reverse = False
    
    i = 0
    while(car.step()):
      next = car.dynamics
      (x_next,y_next) = car.front_position()
      wheel_ticks = next.odometer_ticks - current.odometer_ticks
      if abs(wheel_ticks)>0:
        next_reverse = wheel_ticks < 0
      else:
        next_reverse = reverse
      # skip node if distance threshold hasn't been met
      if distance(x_next,y_next,x,y) < min_length:
        continue

      i = i + 1
      reverse = next_reverse
      
      d = wheel_ticks * car.meters_per_odometer_tick
      wheel_meters_per_second = d / ((next.ms - current.ms) / 1000.)
      
      seconds = (current.ms - start.ms)/1000.
      (rear_x,rear_y) = car.rear_position()
      current = next

      line =  ",".join([
         str(seconds),
         str(x), 
         str(y), 
         str(rear_x),
         str(rear_y),
         str(next_reverse),
#         str(current.odometer_ticks),
 #        str(standardized_degrees(current.heading() - start_heading)),
         str(car.heading_degrees()),
         str(car.heading_adjustment),
         str(current.esc),
         str(current.str),
         str(wheel_meters_per_second)])
      outfile.write(line+"\n")
      print i, line
      
      (x,y) = (x_next,y_next)

       
if __name__ == '__main__':
  input_path = latest_filename('recordings','recording','csv')
  write_path_from_recording_file(input_path)
 
