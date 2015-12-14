class Trajectory:
  self.accel = 5.         # desired acceleration rate
  self.decel = 2.         # desired deceleration rate
  self.speed_limit = 10.  # desired cruise speed
  self.distance = 10.     # distance to travel
  
  # todo: add v_start and v_end parameters for the general
  #       case of connecting segments
  
  def calculate_route(self)
    # calculate assuming we can reach top speed
    t_accel_to_max = self.speed_limit / self.accel
    t_decel_from_max = self.speed_limit / self.decel
    d_accel_to_max = accel * t_accel_to_max ^ 2
    d_decel_from_max = decel * t_decel_from_max ^ 2
    d_cruise = self.distance - d_accel_to_max - d_decel_from_max
    if d_cruise > 0:
      t_cruise = d_cruise / self.speed_limit
      self.t_end_accel = t_accel_to_max
      self.d_end_accel = d_accel_to_max
      self.d_start_decel = self.distance - d_decel_from_max
      self.d_start_decel = self.distance - t_decel_from_max
      return
    
    # we couldn't reach top speed, calculate how long we need 
    # to accelerate to reach goal distance
    #t_accel * accel = t_decl * decel #equal velocity
    #t_decl = t_accel * accel / decel
    #t_accel ^ 2 * accel + t_decl ^ 2 * decl = d
    #t_accel ^ 2 * accel + (t_accel * accel / decel) ^ 2 * decl = d
    #t_accel^2 * accel + t_accel^2 * accel^2 / decel^2 * decl = d
    #t_accel^2 * (accel + accel^2 / decel) = d
    t_accel = sqrt( d / (accel + accel^2 / decl) )
    
     
    


