import time
from Car import Car, angle_diff
car = Car()
time.sleep(3)

goal_heading = car.dynamics.heading+179

while abs(angle_diff(goal_heading, car.dynamics.heading)) > 3:
  car.forward(meters=0.3 , goal_heading = goal_heading)
  time.sleep(0.5)
  car.forward(meters=-0.3, goal_heading = goal_heading)
  time.sleep(0.5)

