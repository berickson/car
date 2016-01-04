from math import *

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
