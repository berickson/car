#include "Logger.h"

bool LOG_ERROR = true;
bool LOG_INFO = false;
bool LOG_TRACE = false;
bool TRACE_RX = false;
bool TRACE_PINGS = false;
bool TRACE_ESC = false;
bool TRACE_MPU = false;
bool TRACE_LOOP_SPEED = false;
bool TD = false;

void log_line(String s) {
  Serial.println(s);
  Serial.send_now();
}



String ftos(float f,int n) {
  return String(f,n);
}
