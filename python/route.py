class RouteNode:
  def __init__:
    self.x = 0.
    self.y = 0.
    
  def set(x,y):
    self.x = x
    self.y = y
  
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
    
  def add_node(x,y):
    node = RouteNode()
    node.set(x=x,y=y)
    nodes.append(node)
  
  def done(self):
    return self.index == len(nodes-2)
    
  
  
  # analyse cross track error at current segment
  # returns (progress, cte), 
  #  progress < 0, you haven't reached start
  #  if progress > 1.0 you have passed the current goal node
  # 
  # inspired by Udacity cs373 quiz_6_6.py and quiz_6_7.py
  def cross_track_error(x,y):
    while True and note self.done():
      # some basic vector calculations
      dx = nodes[index+1].x - nodes[index].x
      dy = nodes[index+1].y - nodes[index].y
      drx = x - nodes[index][0]
      dry = y - nodes[index][1]
      
      # u is the robot estimate projected onto the path segment
      progress = (drx * dx + dry * dy) / (dx * dx + dy * dy)
      
      # the cte is the estimate projected onto the normal of the path segment
      cte = (dry * dx - drx * dy) / (dx * dx + dy * dy)
      
      if progress > 1.0:
        index += 1
      else:
        break

    return None if done else cte

  
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

        self.nodes.append(RouteNode().set_from_standard_file(*fields))
        
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

