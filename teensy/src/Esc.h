#pragma once
#include "Arduino.h"
#include "Servo.h"


class Esc {
public:

  Servo * speed;

  const unsigned long brake_ms = 500;
  const unsigned long pause_ms = 200;
  unsigned long brake_start_ms = 0;
  unsigned long pause_start_ms = 0;

  const int forward_us =  1390;
  const int reverse_us =  1610;


  const int neutral_us = 1500;
  int calibration_us = 0;
  int current_us = -1;

  enum eSpeedCommand : char{
    speed_forward,
    speed_reverse,
    speed_neutral
  };

  static const char * speed_command_name(eSpeedCommand);

  eSpeedCommand command = speed_neutral;

  enum eState {
    stopped,
    forward_braking,
    reverse_braking,
    forward,
    reverse,
    pausing
  } state;

  const char * state_name(eState s);

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
