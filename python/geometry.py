from math import *
import numpy as np

#
# Angular geometry
#

#returns theta in range of [-pi,pi)
def standardized_radians(theta):
  return (theta + pi) % (2.*pi) - pi

#returns theta2-theta1 in range of [-pi,pi)
def radians_diff(theta1, theta2):
  return standardized_radians(theta2 - theta1)


#returns theta in range of [-180,180)
def standardized_degrees(theta):
  return (theta + 180) % 360 - 180

#returns theta2-theta1 in range of [-180,180)
def degrees_diff(theta1, theta2):
  return standardized_degrees(theta2 - theta1)


def dot(x1,y1,x2,y2):
  return x1 * x2 + x2 * y2

def length(x,y):
  return sqrt(x**2+y**2)

#
# General geometry
#

def quadratic(a,b,c):
  return (-b +sqrt(b**2. - 4.*a*c))/(2.*a) ,(-b -sqrt(b**2.-4.*a*c))/(2.*a)

def distance(x1,y1,x2,y2):
  return sqrt( (x2-x1)**2 + (y2-y1)**2 )
  
#
# Kinematics
#

# returns first t greater than zero that will reach position x
def time_at_position(x,a,v0,x0=0.):
  x = float(x) - float(x0)
  v0 = float(v0)
  a = float(a)
  if a==0:
    return x/v0
  t1,t2 = quadratic(0.5*a,v0,-x)
  if t1 < 0:
    return t2 if t2 > 0 else None
  if t2 < 0:
    return t1
  return min(t1,t2)
 
  
def velocity_at_time(t, a, v0):
  t = float(t)
  a = float(a)
  v0 = float(v0)
  return v0 + a * t

def velocity_at_position(x, a, v0, x0 = 0):
  a = float(a)
  v0 = float(v0)
  x = float(x) - float(x0)
  t = time_at_position(x=x,v0=v0,a=a)
  v = velocity_at_time(t=t,a=a,v0=v0)
  return v

# below based on http://stackoverflow.com/a/13849249/383967

def unit_vector(v):
  return v / np.linalg.norm(v)

def angle_between(v1, v2):
  """ Returns the angle in radians between vectors 'v1' and 'v2'::

          >>> angle_between((1, 0, 0), (0, 1, 0))
          1.5707963267948966
          >>> angle_between((1, 0, 0), (1, 0, 0))
          0.0
          >>> angle_between((1, 0, 0), (-1, 0, 0))
          3.141592653589793
  """
  v1_u = unit_vector(v1)
  v2_u = unit_vector(v2)
  angle = np.arccos(np.dot(v1_u, v2_u))
  if np.isnan(angle):
      if (v1_u == v2_u).all():
          return 0.0
      else:
          return np.pi
  return angle


# returns an interpolated value from table of k-v
# kv must be sorted by key
def table_lookup(kv, key):
    last = len(kv)-1
    if key <= kv[0][0]:
      return kv[0][1]
    if key >= kv[last][0]:
      return kv[last][1]
    for i in range(0,last):
      if key <= kv[i+1][0]:
        return interpolate(
          key, kv[i][0], kv[i][1], kv[i+1][0], kv[i+1][1])



#returns y for given x based on x1,y1,x2,y2
def interpolate(x, x1, y1, x2, y2):
  x = float(x)
  x1 = float(x1)
  x2 = float(x2)
  y1 = float(y1)
  y2 = float(y2)
  
  #print("x:{} x1:{} y1:{} x2:{} y2:{}".format(x,x1,y1,x2,y2))
  m = (y2 - y1)/( x2 - x1 )
  y = y1 + m * (x-x1)
  return y



