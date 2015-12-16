                                      

import time
from Car import Car, angle_diff
car = Car()
distance = 200

def test_steering_angle(steering):
  start_heading = car.dynamics.heading
  car.forward(distance, fixed_steering_us = steering)
  after_forward_heading = car.dynamics.heading
  car.forward(-distance, fixed_steering_us = steering)
  after_reverse_heading = car.dynamics.heading
  forward_diff = angle_diff(start_heading, after_forward_heading)
  reverse_diff = angle_diff(after_forward_heading, after_reverse_heading)
  print 'steering: {} fwd_diff: {} rev dif: {} '.format(steering, forward_diff, reverse_diff)


for steering in range(1440, 1480, 5): 
  test_steering_angle(steering)


