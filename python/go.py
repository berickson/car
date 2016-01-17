from car import Car
from play_route import play_route
from route import Route
#car = Car()
meters = 2.0
max_speed = .25


def straight_route():
  route = Route()
  x = 0.1
  while x <= meters + 0.0001:
    route.add_node(x,0.)
    x += 0.05
  while x > 0:
    route.add_node(x,0.,reverse=True)
    x -= 0.05
    
  route.optimize_velocity(max_velocity = max_speed, 
    max_acceleration = 0.5) # 1.0 - safe indoors (3 cm overshoot)
                            # 1.5 - agressive indoors (5 cm overshoot, may slide some)
                            # 2.0 - very agressive indoors (10 cm overshoot)
  return route

route = straight_route()
play_route(route)

#car.forward(meters, max_speed = max_speed)
#car.forward(-meters, max_speed = max_speed)

