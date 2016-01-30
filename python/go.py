#!/usr/bin/env python

#from car import Car
from play_route import play_route
from route import Route, straight_route
#car = Car()


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


route = forward_back_route(distance, max_a, max_v)
play_route(route)

#car.forward(meters, max_speed = max_speed)
#car.forward(-meters, max_speed = max_speed)

