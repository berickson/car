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
    
    reversing = False
    
   
    while(car.step()):
      (x_next,y_next) = car.front_position()
      (rear_x,rear_y) = car.position()
      
      #todo: are we missing terminal node when we switched forward / reverse?
      
      # see if we are switching direction because we always want to write a node if we are
      next_reversing = reversing
      if next.odometer_ticks > current.odometer_ticks:
        next_reversing = False
      if next.odometer_ticks < current.odometer_ticks:
        next_reversing = True
      
      # skip node if distance threshold hasn't been met and haven't switched direction
      if next_reversing == reversing and distance(x_next,y_next,x,y) < min_length:
        continue
      current = next
      (x,y) = (x_next,y_next)
      next = car.dynamics
      reversing = next_reversing
      
      wheel_ticks = next.odometer_ticks - current.odometer_ticks
      d = wheel_ticks * car.meters_per_odometer_tick
      wheel_meters_per_second = d / ((next.ms - current.ms) / 1000.)
      
      seconds = (current.ms - start.ms)/1000.

      line =  ",".join([
         str(seconds),
         str(x), 
         str(y), 
         str(rear_x),
         str(rear_y),
         str(reversing),
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
       

