from math import *

class RouteNode:
  def __init__(self):
    self.x = 0.
    self.y = 0.

  def _repr__(self):
    print "x: {:.2f} y: {:.2f}".format(self.x,self.y)
  
    
  def set(self,x,y,velocity = None):
    self.x = float(x)
    self.y = float(y)
    self.velocity = float(velocity) if velocity is not None else None
  
  def set_from_standard_file(self, secs, x, y, heading_degrees, heading_degrees_adjustment, esc_ms, str_ms, meters_per_second):
    self.secs = secs
    self.x = float(x)
    self.y = float(y)
    self.heading_degrees = float(heading_degrees)
    self.heading_degrees_adjustment = float(heading_degrees_adjustment)
    self.esc_ms = int(esc_ms)
    self.str_ms = int(str_ms)
    self.meters_per_second = float(meters_per_second)

class Route:
  def __init__(self):
    self.columns = ["secs","x","y","heading","adj","esc","str","m/s"];
    self.nodes = []
    # a segment is from node[index] to node[index+1]
    self.index = 0
    
    self.add_node(0,0)
    
  def add_node(self,x,y,velocity=None):
    node = RouteNode()
    node.set(x=x,y=y,velocity=velocity)
    self.nodes.append(node)
  
  def done(self):
    return self.index >= len(self.nodes)-1
    
  
  
  # analyse cross track error at current segment
  # returns (progress, cte), 
  #  progress < 0, you haven't reached start
  #  if progress > 1.0 you have passed the current goal node
  # 
  # inspired by Udacity cs373 quiz_6_6.py and quiz_6_7.py
  def cross_track_error(self,x,y):
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
      cte = (dry * dx - drx * dy) / (dx * dx + dy * dy)
      
      if progress > 1.0:
        self.index += 1
      else:
        break
    if self.done():
      return None
    return cte

  def velocity(self):
    return self.nodes[self.index+1].velocity
  
  def heading_radians(self):
    p1 = self.nodes[self.index]
    p2 = self.nodes[self.index+1]
    return -atan2(p1.y-p2.y,p2.x-p1.x)
    


  
  def header_string(self):
    return ",".join(self.columns)
    
  def load_from_file(self, file_path):
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
        node.set_from_standard_file(*fields)
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
 
 
if __name__ == '__main__':
  route = Route()
  route.load_from_file('recordings/recording_016.csv.path')
  route.save_to_file('deleteme.path')

