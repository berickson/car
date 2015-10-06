#include "Esc.h"
#include "Arduino.h"


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

void Esc::init(Servo * speed) {
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
  if(abs(v)<0.1) {
     us = neutral_us;
  } else if (v > 0) {
    us = min_forward_us + (max_forward_us - min_forward_us ) * v;
  } else if (v < 0) {
    us = min_reverse_us + (max_reverse_us - min_reverse_us ) * abs(v);
  }
  set_pwm_us(us);
}

// sets pulse width, adjusted by calibration if any
void Esc::set_pwm_us(int us) {
  int c_us = us + calibration_us;
  if(c_us != current_us) {
    current_us = c_us;
    speed->writeMicroseconds(current_us);
    Serial.print("ESC set pulse: ");
    Serial.println(current_us);
  }
}

void Esc::set_command(eSpeedCommand new_command) {
  if(command == new_command) {
    return;
  }

  command = new_command;
  Serial.print("ESC Command: ");
  Serial.println(speed_command_name(command));

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
