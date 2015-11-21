#pragma once
#include "Arduino.h"
#include "Servo.h"


class Esc {
public:

  Servo * speed;

  const int min_forward_us = 1560;
  const int max_forward_us = 1600;
  const int min_reverse_us = 1440;
  const int max_reverse_us = 1400;
  
  const int forward_us =  1560;
  const int reverse_us =  1440;


  const int neutral_us = 1500;
  const int deadband_us = 100;
  
  int calibration_us = 0;
  int current_us = -1;
  
  // set speed between -1.0 and 1.0
  void set_velocity(double v);

  enum eSpeedCommand : char{
    speed_forward,
    speed_reverse,
    speed_neutral
  };

  static const char * speed_command_name(eSpeedCommand);

  eSpeedCommand command = speed_neutral;

  // used to match the control stick settings.
  // Will use setting as the new pulse width for
  // neutral
  void set_neutral_pwm_us(unsigned long us);

  void init(Servo * speed);

  // sets pulse width, adjusted by calibration if any
  void set_pwm_us(int us);
  void set_command(eSpeedCommand new_command);
  void execute();
};
