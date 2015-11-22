import time
from Car import Car
car = Car()
time.sleep(1)
heading = car.dynamics.heading
car.forward(800, goal_heading = heading)
time.sleep(3)
car.reverse(800, goal_heading = heading)

