#include "ros.h"
#include "RemoteMode.h"
#include "Arduino.h"
#include "Servo2.h"
#include "PwmInput.h"

extern ros::NodeHandle  nh;

#include "Logger.h"

const int timeout_ms = 1000; // minimum interval to receive commands before ending from timeout

extern Servo2 esc;
extern Servo2 str;

RemoteMode::RemoteMode() {
  name = "remote";
}

void RemoteMode::begin() {
  log(LOG_INFO, "begin of remote mode");
  is_active = true;
  last_command_ms = millis();
  str_us = 1500;
  esc_us = 1500;
  done = false;
}

void RemoteMode::end() {
    Serial.println("end of remote mode");
    str_us = 1500;
    esc_us = 1500;
    update_pulses();
    done = true;
    is_active = false;
}

void RemoteMode::execute() {
  // abort on timeout
  if(millis() - last_command_ms > timeout_ms) {
    end();
  }
}

void RemoteMode::command_steer_and_esc(float _str_us, float _esc_us) {
  if (!is_active) {
    Serial.println("RemoteMode::command_steer_and_esc called when mode inactive, ignoring");
    return;
  }
  last_command_ms = millis();
  str_us = _str_us;
  esc_us = _esc_us;
  update_pulses();
}


void RemoteMode::update_pulses() {
  if(done) return;
  str.writeMicrosecondsFloat(str_us);
  esc.writeMicrosecondsFloat(esc_us);

  // char buffer[200];
  // sprintf(buffer, "str_us: in: %f out: %f esc_us: in: %f  out: %f\n", str_us, str.readMicrosecondsFloat(), esc_us, esc.readMicrosecondsFloat());
  // nh.loginfo(buffer);
}

