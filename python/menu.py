#!/usr/bin/env python2.7
# coding: utf-8
import time
from car import Car
from recorder import make_recording
from make_route import write_path_from_recording_file
from play_route import play_route_main
import Adafruit_CharLCD as LCD

def main():
  car = Car()
  lcd = car.lcd
  buttons = ( (LCD.SELECT, 'Select', (1,1,1)),
              (LCD.LEFT,   'Left'  , (1,0,0)),
              (LCD.UP,     'Up'    , (0,0,1)),
              (LCD.DOWN,   'Down'  , (0,1,0)),
              (LCD.RIGHT,  'Right' , (1,0,1)) )
  while True:
    try:
      car.display_text("{:4.1f}v\n(s)rec  (>) play".format(car.battery_voltage()))
      if lcd.is_pressed(LCD.SELECT):
        print('pressed select')
        car.display_text("recording route\n-> stop")
        make_recording(car = car)
        car.display_text("recording\ncomplete")
        time.sleep(1);
      if lcd.is_pressed(LCD.RIGHT):
        car.display_text("making route")
        write_path_from_recording_file()
        car.display_text("playing route")
        play_route_main()
    except IOError: # I2C error, keep trying
      continue
    except KeyboardInterrupt:
      break
  car.display_text("goodbye")
  print('goodbye')
  time.sleep(1)
  lcd.set_backlight(0)



if __name__ == "__main__":
  main()
