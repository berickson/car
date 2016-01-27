#!/usr/bin/env python

from car import Car
from play_route import play_route
from route import Route
#car = Car()


def straight_route(distance, max_a, max_v):
  route = Route()
  x = 0.1
  while x <= distance + 0.0001:
    route.add_node(x,0.)
    x += 0.05
  while x > 0:
    route.add_node(x,0.,reverse=True)
    x -= 0.05
    
  route.optimize_velocity(max_velocity = max_v, 
    max_acceleration = max_a) # 1.0 - safe indoors (3 cm overshoot)
                            # 1.5 - agressive indoors (5 cm overshoot, may slide some)
                            # 2.0 - very agressive indoors (10 cm overshoot)
  return route


import argparse
parser = argparse.ArgumentParser(description = 'Route follower')
parser.add_argument(
  '--distance',
  nargs='?',
  type=float,
  default=3.0,
  help='distance')
parser.add_argument(
  '--max_a',
  nargs='?',
  type=float,
  default=1.0,
  help='max_acceleration')
parser.add_argument(
  '--max_v',
  nargs='?',
  type=float,
  default='1.0',
  help='max_velocity')
args = parser.parse_args()
max_a = args.max_a
max_v = args.max_v
distance = args.distance


route = straight_route(distance, max_a, max_v)
play_route(route)

#car.forward(meters, max_speed = max_speed)
#car.forward(-meters, max_speed = max_speed)

