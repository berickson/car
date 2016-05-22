import numpy as np                                      
import time
from Car import Car, angle_diff
car = Car()
distance = 200

# finds the zero value by doing a first order fit
def find_zero(x,y):
  m,b=np.polyfit(x,y,1)
  return -b/m


def test_steering_angle(steering):
  start_heading = car.dynamics.heading
  car.forward(distance, fixed_steering_us = steering)
  after_forward_heading = car.dynamics.heading
  car.forward(-distance, fixed_steering_us = steering)
  after_reverse_heading = car.dynamics.heading
  forward_diff = angle_diff(start_heading, after_forward_heading)
  reverse_diff = angle_diff(after_forward_heading, after_reverse_heading)
  print 'steering: {} fwd_diff: {} rev dif: {} '.format(steering, forward_diff, reverse_diff)
  return forward_diff, reverse_diff

steering_input =  range(1440, 1480, 10)
forward_results = []
reverse_results = []
for steering in steering_input: 
  (forward_result, reverse_result) = test_steering_angle(steering)
  forward_results.append(forward_result)
  reverse_results.append(reverse_result)

forward_center = find_zero(steering_input, forward_results)
reverse_center = find_zero(steering_input, reverse_results)


print 'forward center: {}'.format(forward_center)
print 'reverse center: {}'.format(reverse_center)

answer = raw_input('Write to config file: [Y/n]')
if len(answer)==0  or answer[0].lower() == 'y':
  car.set_option('calibration', 'center_steering_us', int(forward_center))
  car.set_option('calibration', 'reverse_center_steering_us', int(reverse_center))



