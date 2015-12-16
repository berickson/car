import time
from Car import Car
car = Car()
distance = 500
#car.set_rc_mode()
car.forward(distance, fixed_steering_us = 1460)
car.forward(-distance)
car.set_manual_mode()
