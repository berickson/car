#include "ManualMode.h"
#include "Arduino.h"
#include "Servo2.h"
#include "PwmInput.h"
#include "Logger.h"

extern Servo2 esc;
extern Servo2 str;
extern PwmInput rx_str;
extern PwmInput rx_esc;

ManualMode::ManualMode() {
  name = "manual";
}

void ManualMode::begin() {
  log(LOG_INFO, "begin of manual mode");
}

void ManualMode::end() {
    str.writeMicroseconds(1500);
    esc.writeMicroseconds(1500);
}

void ManualMode::execute() {
  if(rx_str.pulse_us() > 0 && rx_esc.pulse_us() > 0) {
    str.writeMicroseconds(rx_str.pulse_us());
    esc.writeMicroseconds(rx_esc.pulse_us());
  } else {
    esc.writeMicroseconds(1500);
    str.writeMicroseconds(1500);
  }
}

