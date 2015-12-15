import time
from Car import Car
car = Car()
time.sleep(1)
distance = 500
heading = car.dynamics.heading
#car.set_rc_mode()
car.forward(distance, goal_heading = heading)
car.forward(-distance, goal_heading = heading)
#car.set_manual_mode()
