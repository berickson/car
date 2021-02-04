#include "Logger.h"

bool LOG_ERROR = true;
bool LOG_INFO = true;
bool LOG_TRACE = false;
bool TRACE_RX = false;
bool TRACE_PINGS = false;
bool TRACE_MPU = false;
bool TRACE_LOOP_SPEED = false;
bool TRACE_TELEMETRY = false;
bool TD = true;
bool TD2 = true;

void log_line(String s) {
  return;
  Serial.println(s);
  Serial.send_now();
}

