import sys,tty,termios
import datetime
import time
import dateutil.parser
from select import select
from car import Car, Dynamics, degrees_diff
from recording import Recording

desired_velocity = 0.3

def play_route():
  route = Route()
  route.add_node(0.,0.)
  route.add_node(1.,0.)
  route.add_node(2.,0.5)
  
  car = Car():
  car.set_rc_mode()

  # keep going until we run out of track  
  while True:
    (x,y) = car.position()
    cte = route.cross_track_error(x,y)
    if cte is None:
      break;

    # calculate speed 
    if car.velocity < velocity:
      esc_ms = car.min_forward_speed
    else:
      esc_ms = 1500
      
    # calculate steering
    str_ms = car.steering_for_angle(30 * cte)
    car.set_speed_and_steering(str_ms, esc_ms)
    
    
    time.sleep(0.02)

  car.set_manual_mode()
    
  
if __name__ == '__main__':
  play_route()
  
