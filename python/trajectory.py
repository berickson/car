import math

class Trajectory:
  def __init__(self, accel = 1, decel = 1, speed_limit = 1, distance = 10):
    self.accel = float(accel)             # desired acceleration rate
    self.decel = float(decel)             # desired deceleration rate
    self.speed_limit = float(speed_limit) # desired cruise speed
    self.distance = float(distance)       # distance to travel
    self.t_end_accel = float('nan')
    self.d_end_accel = float('nan')
    self.t_start_decel = float('nan')
    self.d_start_decel = float('nan')
    self.t_end = float('nan')
    
  def __repr__(self):
    f = ("accel: {}\n"
         "decel: {}\n"
         "speed_limit: {}\n"
         "distance: {}\n"
         "t_end_accel: {}\n"
         "d_end_accel: {}\n"
         "t_start_decl: {}\n"
         "d_start_decl: {}\n"
         "t_end: {}")
    return f.format(
        self.accel,
        self.decel,
        self.speed_limit,
        self.distance,
        self.t_end_accel,
        self.d_end_accel,
        self.t_start_decel,
        self.d_start_decel,
        self.t_end
        )
  
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
      self.d_end_accel = d_accel_to_max
      self.d_start_decel = self.distance - d_decel_from_max
      self.t_start_decel = t_accel_to_max + t_cruise
      self.t_end = t_accel_to_max + t_cruise + t_decel_from_max
      return
    
    # we couldn't reach top speed, calculate how long we need 
    # to accelerate to reach goal distance
    #t_accel * accel = t_decl * decel #equal velocity
    #t_decl = t_accel * accel / decel
    #t_accel ^ 2 * accel + t_decl ^ 2 * decl = d
    #t_accel ^ 2 * accel + (t_accel * accel / decel) ^ 2 * decl = d
    #t_accel^2 * accel + t_accel^2 * accel^2 / decel^2 * decl = d
    #t_accel^2 * (accel + accel^2 / decel) = d
    t_accel = math.sqrt( self.distance / (self.accel + self.accel ** 2. / self.decel) )
    self.t_end_accel = t_accel
    self.d_end_accel = self.accel * t_accel ** 2.
     
    self.t_start_decel = self.t_end_accel 
    self.d_start_decel = self.d_end_accel
    t_decel = (self.distance - self.d_start_decel)/self.accel
    self.t_end = self.t_start_decel + t_decel

def test(*args, **kwargs):
  t = Trajectory(*args, **kwargs)
  t.calculate_route()
  print("--------------")
  print(str(t))
  print("--------------")
  
test()
test(accel = 5)
test(accel=5, decel=5, speed_limit = 1)
test(distance=1)
