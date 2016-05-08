from math import *
from geometry import *
import numpy as np


def reverse_route(distance, max_a, max_v):
  route = Route()
  x = 0.0
  while x > -distance:
    route.add_node(x,0.,reverse=True)
    x -= 0.05
  route.add_node(-distance,0.,reverse=True)
    
  route.optimize_velocity(max_velocity = max_v, 
    max_acceleration = max_a) # 1.0 - safe indoors (3 cm overshoot)
                            # 1.5 - agressive indoors (5 cm overshoot, may slide some)
                            # 2.0 - very agressive indoors (10 cm overshoot)
  return route

def forward_back_route(distance, max_a, max_v):
  route = Route()
  x = 0.1
  while x <= distance + 0.0001:
    route.add_node(x,0.)
    x += 0.05
  while x > 0:
    route.add_node(x,0.,reverse=True)
    x -= 0.05
    
  route.optimize_velocity(max_velocity = max_v, 
    max_acceleration = max_a) # 1.0 - safe indoors (3 cm overshoot)
                            # 1.5 - agressive indoors (5 cm overshoot, may slide some)
                            # 2.0 - very agressive indoors (10 cm overshoot)
  return route


class RouteNode:
  def __init__(self):
    self.x = 0.
    self.y = 0.
    self.reverse = False


  def __repr__(self):
    return "x: {:.2f} y: {:.2f} velocity: {:2f} reverse: {}".format(self.x,self.y, self.velocity, self.reverse)
  
    
  def set(self,x,y,velocity = None, reverse = False):
    self.x = float(x)
    self.y = float(y)
    self.reverse = reverse
    self.velocity = float(velocity) if velocity is not None else None
  
  def set_from_standard_file(self, secs, x, y, rear_x, rear_y, reverse, heading_degrees, heading_degrees_adjustment, esc_ms, str_ms, meters_per_second, velocity = 1.0):
    self.secs = float(secs)
    self.x = float(x)
    self.y = float(y)
    self.rear_x = float(rear_x)
    self.rear_y = float(rear_y)
    self.reverse = (reverse == 'True')
    self.heading_degrees = float(heading_degrees)
    self.heading_degrees_adjustment = float(heading_degrees_adjustment)
    self.esc_ms = int(esc_ms)
    self.str_ms = int(str_ms)
    self.meters_per_second = float(meters_per_second)
    self.velocity = float(velocity)

class Route:
  def __init__(self):
    self.columns = ["secs","x","y","rear_x", "rear_y", "reverse", "heading","adj","esc","str","m/s"];
    self.nodes = []
    # a segment is from node[index] to node[index+1]
    self.index = 0
    self.segment_progress = 0.
    
    self.add_node(0.,0.)
    self._done = False
    
  def __repr__(self):
    return "\n".join([ (str(i)+" " + str(self.nodes[i])) for i in range(len(self.nodes))])
    
  def add_node(self,x,y,velocity=1.0, reverse=False):
    node = RouteNode()
    node.set(x=x,y=y,velocity=velocity,reverse=reverse)
    self.nodes.append(node)
  
  def done(self):
    return self._done
    
  
  def cross_track_error(self):
    return self.cte
  
  
#  def segment_progress(self):
#    if self.done():
#      return 1.
#    return self.segment_progress
  
  
  # analyse cross track error at current segment
  #
  # positive cte means you are to the left of track
  # 
  # inspired by Udacity cs373 quiz_6_6.py and quiz_6_7.py
  def set_position(self,x,y,rear_x,rear_y, velocity):
    cte = 0.0
    while True:
      p1 = self.nodes[self.index]
      p2 = self.nodes[self.index+1]

      #print 'set_position velocity: {} p2.velocity {}: '.format(velocity, p2.velocity)
      # skip this node if we reach the required zero velocity for
      # a non-terminal node so we can get moving a again
      if abs(velocity) <0.01 and abs(p2.velocity) <0.01:
        progress = 1.0
      else:

        if p1.reverse:
          if hasattr(p1,'rear_x'):
            dx = p2.rear_x - p1.rear_x
            dy = p2.rear_y - p1.rear_y
            drx = rear_x - p1.rear_x
            dry = rear_y - p1.rear_y
          else:
            h = self.heading_radians()
            l = distance(x,y,rear_x,rear_y)
            rear_offset_x = -l * cos(h)
            rear_offset_y = -l * sin(h)
            dx = p2.x-p1.x
            dy = p2.y-p1.y
            drx = rear_x - (p1.x + rear_offset_x)
            dry = rear_y - (p1.y + rear_offset_y)
            
        else:
          # some basic vector calculations
          dx = p2.x - p1.x
          dy = p2.y - p1.y
          drx = x - p1.x
          dry = y - p1.y
        
        # u is the robot estimate projected onto the path segment
        progress = (drx * dx + dry * dy) / (dx * dx + dy * dy)
        
        # the cte is the estimate projected onto the normal of the path segment
        cte = (dry * dx - drx * dy) / sqrt(dx * dx + dy * dy)
      
      
      if progress < 1.0 or self.done():
        break
      if self.index >= len(self.nodes)-2.:
        self._done = True
        break
      else:
        self.index += 1
    self.segment_progress = progress
    self.cte = cte
  
  # returns (x,y) position of the point at distance d ahead of the current position
  def get_position_ahead(self, d):
    d = float(d)
    i = self.index
    done = False
    segment_progress = self.segment_progress
    while not done:
      p1 = self.nodes[i]
      p2 = self.nodes[i+1]
      dx = p2.x-p1.x
      dy = p2.y-p1.y
      l = sqrt(dx*dx+dy*dy)
      progress_d = l * segment_progress
      remaining_d = l - progress_d
      # go to next node 
      # if we are past the end of this node 
      # and there are nodes left
      if d > remaining_d and i < len(self.nodes)-2:
        i+=1
        d -= remaining_d
        segment_progress = 0.
        continue
      unit_x = dx/l
      unit_y = dy/l
      x = p1.x + segment_progress * dx
      y = p1.y + segment_progress * dy
      ahead_x = x+unit_x * d
      ahead_y = y+unit_y * d
      done = True
    return (ahead_x,ahead_y)

  def velocity(self):
    if self.done(): return 0.
    p0 = self.nodes[self.index]
    p1 = self.nodes[self.index+1]
    
    # if we just turned around, take the velocity from the node in the new direction
    if self.index > 0 and self.nodes[self.index-1].reverse != p1.reverse:
      v = p1.velocity
    
    # interpolate speed
    elif(self.segment_progress < 0):
      v =  p0.velocity
    elif(self.segment_progress > 1):
      v= p1.velocity
    else:
      v = p0.velocity + (p1.velocity - p0.velocity) * self.segment_progress
    
    # set negative speed for reverse
    if p0.reverse:
      v = -v
    return v

  def heading_radians(self):
    p1 = self.nodes[self.index]
    p2 = self.nodes[self.index+1]
    h = -atan2(p1.y-p2.y,p2.x-p1.x)
    if p1.reverse:
      h += pi
    return standardized_radians(h)
  
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
  
  def is_reverse(self):
    return self.nodes[self.index].reverse
        
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
    #self.nodes[0].velocity = 1
    self.nodes[-1].velocity = 0.
    iterations = 0
    
    # apply speed limit of zero for switching direction
    for i in range(len(self.nodes)-2):
      p0 = self.nodes[i]
      p1 = self.nodes[i+1]
      if p0.reverse != p1.reverse:
        p1.velocity = 0.0
    
    
    # apply speed limit for curves
    for i in range(len(self.nodes)-3):
      p0 = self.nodes[i]
      p1 = self.nodes[i+1]
      p2 = self.nodes[i+2]

      s1 = np.array([p1.x-p0.x, p1.y-p0.y])
      s2 = np.array([p2.x-p1.x, p2.y-p1.y])
      theta = abs(angle_between(s1,s2))

      # assume r = |s2| * theta (seems ok but why?) todo: find exact answer here
      if theta > 0:
        r = abs(length(*s2) / theta)
        v_circle_max = sqrt(max_acceleration * r)
        #print 'i: {} theta: {} r: {} v_circle_max: {}'.format(i,theta, r,v_circle_max)
        p1.velocity = min(p1.velocity, v_circle_max)
      
     

    for i in reversed(range(0,len(self.nodes)-1)):
      p0 = self.nodes[i]
      p1 = self.nodes[i+1]
      dv = abs(p1.velocity - p0.velocity)
      ds = distance(p0.x,p0.y,p1.x,p1.y)
      
      min_v = p1.velocity 
      max_dv = velocity_at_position(x=ds, a=max_acceleration, v0=min_v) - min_v
      
      if dv > max_dv + tolerance: 
        p0.velocity = min(p0.velocity, velocity_at_position(x=ds, a=max_acceleration, v0=p1.velocity))
    
 
 
def test_straight_line():
  print 'testing a straight line'
  route = Route()
  for i in range(1,11):
    route.add_node(i,0)
  route.optimize_velocity()
  print(route)

def test_reversing():
  print 'testing a back and forth line'
  route = Route()
  for i in range(1,11):
    route.add_node(i,0)
  for i in range(11,21):
    route.add_node(i,0,reverse=True)
  route.optimize_velocity()
  print(route)

def get_circle_route():
  route = Route()
  steps = 30
  r = 1
  for i in range(1,steps):
    theta = 2*pi * i / steps
    x = r * sin(theta)
    y = r - r * cos(theta)
    route.add_node(x,y)
  return route

def test_circle():
  print 'testing a circle'
  route = get_circle_route()
  route.optimize_velocity()
  print(route)

def test_position_ahead():
  print 'testing position_ahead with a circle'
  route = get_circle_route()
  print 'd,x,y'
  for d in np.arange(0,10,0.1):
    x,y = route.get_position_ahead(d)
    print d,x,y
  
 
if __name__ == '__main__':
  test_position_ahead()
  test_circle()
  test_straight_line()
  test_reversing()

