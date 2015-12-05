import time
from Car import Car
car = Car()

sleep_time = 5.0
print "waiting {} seconds for things to settle".format(sleep_time)
time.sleep(sleep_time)


neutral_speed = 1500
neutral_steer = 1500

increment = 10
wait_seconds = 2.0
move_threshold = 1.0



def does_it_move(speed):
  car.set_rc_mode()
  print 'does it move at {}?'.format(speed)
  car.set_speed_and_steering(speed,neutral_steer)
  current_test_start_time = time.time()
  moved = False
  start_inches = car.dynamics.ping_inches;
  print "current ping is {} inches".format(start_inches)
  while time.time() - current_test_start_time < wait_seconds and not moved:
    car.set_speed_and_steering(speed,neutral_steer)
    time.sleep(0.1)
    new_inches = car.dynamics.ping_inches
    print "current ping is {} inches".format(new_inches)
    if abs(new_inches - start_inches) > move_threshold:
      moved = True
  car.set_speed_and_steering(neutral_speed,neutral_steer)
  car.set_manual_mode()
  return moved



def calibrate_min_forward_speed():
  speed = 1540
  moved = False
  while not moved:
    moved = does_it_move(speed)
    if not moved:
      speed+=1
  time.sleep(3) # allow to stop completely
  return speed
  
def calibrate_min_reverse_speed():
  time.sleep(3)

  speed = 1460
  moved = False
  while not moved:
    moved = does_it_move(speed)
    if not moved:
      speed-=1
  return speed


min_forward_speed = calibrate_min_forward_speed()
min_reverse_speed = calibrate_min_reverse_speed()

print 'min_forward_speed:  {}  min_reverse_speed: {}'.format(min_forward_speed, min_reverse_speed)

