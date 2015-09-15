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

const char * Esc::state_name(eState s) {
  const char *names[]  = {
    "stopped",
    "forward_braking",
    "reverse_braking",
    "forward",
    "reverse",
    "pausing"
  };
  return names[s];
}

// used to match the control stick settings.
// Will use setting as the new pulse width for
// neutral
void Esc::set_neutral_pwm_us(unsigned long us) {
  calibration_us = us - neutral_us;
}

void Esc::init(Servo * speed) {
  this->speed = speed;
  state = stopped;
  set_pwm_us(neutral_us);
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
  Serial.print("OLD ESC State: ");
  Serial.println(state_name(state));

  if(command == speed_forward) {
    switch(state) {
      case forward_braking:
      case stopped:
      case forward:
        state = forward;
        set_pwm_us(forward_us);
        break;

      case pausing:
        break;

      case reverse_braking:
        set_pwm_us(forward_us);
        break;

      case reverse:
        set_pwm_us(forward_us);
        state = reverse_braking;
        brake_start_ms = millis();
        break;
    }
  }
  if(command == speed_reverse) {
    switch(state) {
      case reverse_braking:
      case stopped:
      case reverse:
        state = reverse;
        set_pwm_us(reverse_us);
        break;

      case pausing:
        break;

      case forward_braking:
        set_pwm_us(reverse_us);
        break;

      case forward:
        state = forward_braking;
        set_pwm_us(reverse_us);
        brake_start_ms = millis();
        break;
    }
  }
  if(command == speed_neutral){
    switch(state) {
      case stopped:
      case pausing:
        set_pwm_us(neutral_us);
        break;

      case forward_braking:
        set_pwm_us(reverse_us);
        break;

      case reverse_braking:
        set_pwm_us(forward_us);
        break;

      case reverse:
        state = reverse_braking;
        set_pwm_us(forward_us);
        brake_start_ms = millis();
        break;

      case forward:
        state = forward_braking;
        set_pwm_us(reverse_us);
        brake_start_ms = millis();
        break;
    }
  }
  Serial.print("New ESC State: ");
  Serial.println(state_name(state));
}

void Esc::execute() {
  if(state == forward_braking || state == reverse_braking) {
    unsigned long ms = millis();
    if(ms - brake_start_ms >= brake_ms) {
      Serial.println("Braking complete");
      state = pausing;
      pause_start_ms = ms;
      set_pwm_us(neutral_us);
    }
  }

  if(state == pausing) {
    unsigned long ms = millis();
    if(ms - pause_start_ms >= pause_ms) {
      Serial.println("Pausing complete");
      state = stopped;
    }
  }

  if(state == stopped) {
    if(command == speed_forward) {
      Serial.println("forward from stopped");
      set_pwm_us(forward_us);
      state = forward;
    }
    if(command == speed_reverse) {
      Serial.println("reverse from stopped");
      set_pwm_us(reverse_us);
      state = reverse;
    }
    if(command == speed_neutral) {
      set_pwm_us(neutral_us);
      state = stopped;
    }
  }
}
