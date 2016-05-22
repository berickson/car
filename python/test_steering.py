import time
from car import Car
car = Car()


try:
  car.set_rc_mode()
  
  steer_angle = 0.0
  max_angle = 30.
  min_angle = -max_angle
  step_seconds = 0.02
  step = max_degrees_per_second * step_seconds
  
  
  while True:
      steer_angle += step
      if steer_angle >= max_angle:
        step = -abs(step)
      if steer_angle <= min_angle:
        step = abs(step)
      steer_ms = car.steering_for_angle(steer_angle)
      car.set_esc_and_str(1500, steer_ms)
      time.sleep(0.02)
finally:
  car.set_manual_mode()
