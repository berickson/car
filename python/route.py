from math import *

class RouteNode:
  def __init__(self):
    self.x = 0.
    self.y = 0.


  def __repr__(self):
    return "x: {:.2f} y: {:.2f} velocity: {:2f}".format(self.x,self.y, self.velocity)
  
    
  def set(self,x,y,velocity = None):
    self.x = float(x)
    self.y = float(y)
    self.velocity = float(velocity) if velocity is not None else None
  
  def set_from_standard_file(self, secs, x, y, heading_degrees, heading_degrees_adjustment, esc_ms, str_ms, meters_per_second, velocity = 1.0):
    self.secs = secs
    self.x = float(x)
    self.y = float(y)
    self.heading_degrees = float(heading_degrees)
    self.heading_degrees_adjustment = float(heading_degrees_adjustment)
    self.esc_ms = int(esc_ms)
    self.str_ms = int(str_ms)
    self.meters_per_second = float(meters_per_second)
    self.velocity = float(velocity)

class Route:
  def __init__(self):
    self.columns = ["secs","x","y","heading","adj","esc","str","m/s"];
    self.nodes = []
    # a segment is from node[index] to node[index+1]
    self.index = 0
    self.progress = 0.
    
    self.add_node(0,0)
    
  def __repr__(self):
    return "\n".join([str(node) for node in self.nodes])
    
  def add_node(self,x,y,velocity=None):
    node = RouteNode()
    node.set(x=x,y=y,velocity=velocity)
    self.nodes.append(node)
  
  def done(self):
    return self.index >= len(self.nodes)-1
    
  
  def cross_track_error(self):
    if self.done():
      return None
    return self.cte
  
  
  def segment_progress(self):
    if self.done():
      return 1.
    return self.segment_progress
  
  
  # analyse cross track error at current segment
  #
  # positive cte means you are to the left of track
  # 
  # inspired by Udacity cs373 quiz_6_6.py and quiz_6_7.py
  def set_position(self,x,y):
    cte = 0.0
    while not self.done():
      p1 = self.nodes[self.index]
      p2 = self.nodes[self.index+1]
      # some basic vector calculations
      dx = p2.x - p1.x
      dy = p2.y - p1.y
      drx = x - p1.x
      dry = y - p1.y
      
      # u is the robot estimate projected onto the path segment
      progress = (drx * dx + dry * dy) / (dx * dx + dy * dy)
      
      # the cte is the estimate projected onto the normal of the path segment
      cte = (dry * dx - drx * dy) / sqrt(dx * dx + dy * dy)
      
      if progress > 1.0:
        self.index += 1
      else:
        break
    self.segment_progress = progress
    self.cte = cte

  def velocity(self):
    p0 = self.nodes[self.index]
    p1 = self.nodes[self.index+1]
    if(self.position < 0):
      return p0.velocity
    if(self.position > 0):
      return p1.velocity
      
    return p0.velocity + (p1.velocity - p0.velocity) * self.progress

  def heading_radians(self):
    p1 = self.nodes[self.index]
    p2 = self.nodes[self.index+1]
    return -atan2(p1.y-p2.y,p2.x-p1.x)
    


  
  def header_string(self):
    return ",".join(self.columns)
    
  def load_from_file(self, file_path, velocity = 1.0):
    line_number = 1
    with open(file_path,'r') as infile:
      # read first line and make sure it matches expected format
      if infile.readline().strip() != self.header_string():
        raise Exception("file doesn't match expected format")

      while True:
        line_number = line_number + 1
        l = infile.readline().strip()
        if l == "":
          break
        fields = l.split(',')
        if len(fields) != len(self.columns):
          raise Exception("wrong number of columns in data on line {}, expected {} was {}".format(line_number, len(self.columns), len(fields)));
        node = RouteNode()
        node.set_from_standard_file(*fields, velocity = velocity)
        self.nodes.append(node)
    self.index = 0
        
  def save_to_file(self, file_path):
    with open(file_path,'w') as f:
      header = ','.join(self.columns)
      f.write(header+'\n')
      for node in self.nodes:
        line = ",".join(
          [str(node.secs),
          str(node.x),
          str(node.y),
          str(node.heading_degrees),
          str(node.heading_degrees_adjustment),
          str(node.esc_ms),
          str(node.str_ms),
          str(node.meters_per_second)]
          )
        f.write(line+'\n')

  def optimize_velocity(self, max_velocity = 1., max_acceleration = 0.1):
    tolerance = 0.000001
    max_velocity = float(max_velocity)
     
    # start everything at max_velocity
    for node in self.nodes:
      node.velocity = max_velocity
     
    # set start and end velocities
    self.nodes[0].velocity = 0.1
    self.nodes[-1].velocity = 0.
    iterations = 0
     
    changed = True
    while changed:
      changed = False
      iterations += 1
       
      # make range to go up the list and back down
      forward = range(0,len(self.nodes)-1)
      backward = list(forward)
      backward.reverse()
      forward_and_back = list(forward)
      forward_and_back.extend(backward[:])

      for i in forward_and_back:
        p0 = self.nodes[i]
        p1 = self.nodes[i+1]
        dv = abs(p1.velocity - p0.velocity)
        if dv > max_acceleration + tolerance: # todo make this a real accelaration calculation
          p0.velocity = min(p0.velocity, p1.velocity + max_acceleration)
          p1.velocity = min(p1.velocity, p0.velocity + max_acceleration)
          changed = True
    print 'calculated route velocity in {} iterations'.format(iterations)
    
 
 
if __name__ == '__main__':
#  route = Route()
#  route.load_from_file('recordings/recording_016.csv.path')
#  route.save_to_file('deleteme.path')
  
  route = Route()
  for i in range(1,11):
    route.add_node(i,0)
  route.optimize_velocity()
  print(route)


