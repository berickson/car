import math
import numpy as np

class trajectory_state:
  def __init__(self):
    self.t = float('nan');
    self.x = float('nan');
    self.v = float('nan');
    self.a = float('nan');
    
  def __repr__(self):
    return "t: {:<5} x: {:<5.3} v: {:<5.3} a: {:<5}".format(self.t,self.x,self.v,self.a)
  
class Trajectory:
  def __init__(self, accel = 1, decel = 1, speed_limit = 1, distance = 3):
    self.accel = float(accel)             # desired acceleration rate
    self.decel = float(decel)             # desired deceleration rate
    self.speed_limit = float(speed_limit) # desired cruise speed
    self.distance = float(distance)       # distance to travel
    self.t_end_accel = float('nan')
    self.x_end_accel = float('nan')
    self.t_start_decel = float('nan')
    self.x_start_decel = float('nan')
    self.t_end = float('nan')
    
  def __repr__(self):
    f = ("accel: {}\n"
         "decel: {}\n"
         "speed_limit: {}\n"
         "distance: {}\n"
         "t_end_accel: {}\n"
         "x_end_accel: {}\n"
         "t_start_decl: {}\n"
         "x_start_decl: {}\n"
         "t_end: {}")
    return f.format(
        self.accel,
        self.decel,
        self.speed_limit,
        self.distance,
        self.t_end_accel,
        self.x_end_accel,
        self.t_start_decel,
        self.x_start_decel,
        self.t_end
        )
  
  def state_at_time(self,t):
    s = trajectory_state()
    s.t=t
    if t < 0.:
      s.x =0.
      s.a = 0.
      s.v = 0.
      return s
    if t > self.t_end:
      s.x = self.distance
      s.a = 0.
      s.v = 0.
      return s
    if t >= self.t_start_decel:
      s.x = self.distance - 0.5 * self.decel * (self.t_end - t) ** 2
      s.a = -self.decel
      s.v = (self.t_end - t) * self.decel
      return s
    if t >= self.t_end_accel:
      s.x = self.x_end_accel + self.speed_limit * t-self.t_end_accel
      s.a = 0.0
      s.v = self.speed_limit
      return s
    # if we get here, we must be in first phase of accelerating
    s.x = 0.5 * self.accel * t ** 2
    s.a = self.accel
    s.v = self.accel * t
    return s
  
  # todo: add v_start and v_end parameters for the general
  #       case of connecting segments
  def calculate_route(self):
    # calculate assuming we can reach top speed
    t_accel_to_max = self.speed_limit / self.accel
    t_decel_from_max = self.speed_limit / self.decel
    d_accel_to_max = 0.5 * self.accel * t_accel_to_max ** 2.
    d_decel_from_max = 0.5 * self.decel * t_decel_from_max ** 2.
    d_cruise = self.distance - d_accel_to_max - d_decel_from_max
    if d_cruise > 0.:
      t_cruise = d_cruise / self.speed_limit
      self.t_end_accel = t_accel_to_max
      self.x_end_accel = d_accel_to_max
      self.x_start_decel = self.distance - d_decel_from_max
      self.t_start_decel = t_accel_to_max + t_cruise
      self.t_end = t_accel_to_max + t_cruise + t_decel_from_max
      return
    
    # we couldn't reach top speed, calculate how long we need 
    # to accelerate to reach goal distance
    #t_accel * accel = t_decel * decel #equal velocity
    #t_decel = t_accel * accel / decel
    #0.5 * t_accel ^ 2 * accel + 0.5 * t_decel ^ 2 * decel = d
    #t_accel ^ 2 * accel + (t_accel * accel / decel) ^ 2 * decel = 2 d
    #t_accel^2 * accel + t_accel^2 * accel^2 / decel^2 * decel = 2 d
    #t_accel^2 * (accel + accel^2 / decel) = 2 d
    t_accel = math.sqrt( 2. * self.distance / (self.accel + self.accel ** 2. / self.decel) )
    self.t_end_accel = t_accel
    self.x_end_accel = 0.5 * self.accel * t_accel ** 2.
    v_max = self.t_end_accel * self.accel
     
    self.t_start_decel = self.t_end_accel 
    self.x_start_decel = self.x_end_accel
    t_decel = (v_max)/self.decel
    self.t_end = self.t_start_decel + t_decel

def test(*args, **kwargs):
  tragectory = Trajectory(*args, **kwargs)
  tragectory.calculate_route()
  print("--------------")
  print(str(tragectory))
  print("--------------")
  for t in np.arange(0.,tragectory.t_end+0.1, 0.1):
    print (tragectory.state_at_time(t))
  print("--------------")

def main():
  test(accel = 2.8, decel=0.3, speed_limit = 1)

if __name__ == "__main__":
  main()
