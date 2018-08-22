#include "Esc.h"
#include "Arduino.h"
#include "Logger.h"


const char * Esc::speed_command_name( Esc::eSpeedCommand e) {
  const char * names[] = {
    "speed_forward",
    "speed_reverse",
    "speed_neutral"

  };
  return names[e];
}


// used to match the control stick settings.
// Will use setting as the new pulse width for
// neutral
void Esc::set_neutral_pwm_us(unsigned long us) {
  calibration_us = us - neutral_us;
}

void Esc::init(Servo2 * speed) {
  this->speed = speed;
  set_pwm_us(neutral_us);
}

// set velocity between -1.0 and 1.0
void Esc::set_velocity(double v) {
  // clamp
  if(v < -1.0) {
    v = -1.0;
  } else if (v  > 1.0) {
    v = 1.0;
  } 
  
  // set speeds
  unsigned long us = neutral_us;
  const double neutral_v = 0.1; // small numbers are considered zero
  if(abs(v)<neutral_v) {
     us = neutral_us;
  } else if (v > 0) {
    us = min_forward_us + (max_forward_us - min_forward_us ) * (v-neutral_v);
  } else if (v < 0) {
    us = min_reverse_us + (max_reverse_us - min_reverse_us ) * (abs(v)-neutral_v);
  }
  set_pwm_us(us);
}

// sets pulse width, adjusted by calibration if any
void Esc::set_pwm_us(int us) {
  int c_us = us + calibration_us;
  if(c_us != current_us) {
    current_us = c_us;
    speed->writeMicroseconds(current_us);
    log(TRACE_ESC, "ESC set pulse: " + current_us);
  }
}

void Esc::set_command(eSpeedCommand new_command) {
  if(command == new_command) {
    return;
  }

  command = new_command;
  log(TRACE_ESC, "ESC Command: " + speed_command_name(command));

  if(command == speed_forward) {
      set_pwm_us(forward_us);
  }
  if(command == speed_reverse) {
      set_pwm_us(reverse_us);
  }
  if(command == speed_neutral){
      set_pwm_us(neutral_us);
  }
}
  
void Esc::execute() {
}
