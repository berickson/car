from math import *

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
