import time
from Car import Car
car = Car()
meters = 1.0
#car.set_rc_mode()
car.forward(meters, fixed_steering_us = 1460)
car.forward(-meters)
car.set_manual_mode()
