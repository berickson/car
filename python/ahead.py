import time
from car import Car
from geometry import *

#goal is to find a way to point the wheels straight ahead

car = Car()
time.sleep(1)

original_heading = car.dynamics.heading
car.set_rc_mode()
speed = 1500
while(True):
  heading = angle_diff(original_heading, car.dynamics.heading)
  steering = car.steering_for_angle(-heading)
  car.set_speed_and_steering(speed, steering)
  #print "Heading: {} Steering: {}".format(heading, steering)
  time.sleep(0.01)
car.set_manual_mode()
