import time
from Car import Car
car = Car()
time.sleep(1)
heading = car.dynamics.heading
#car.set_rc_mode()
car.forward(500, goal_heading = heading)
time.sleep(3)
car.reverse(500, goal_heading = heading)
#car.set_manual_mode()
