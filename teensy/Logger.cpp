#include "Logger.h"

bool LOG_ERROR = true;
bool LOG_INFO = false;
bool LOG_TRACE = false;
bool TRACE_RX = false;
bool TRACE_PINGS = false;
bool TRACE_ESC = false;
bool TRACE_MPU = false;
bool TRACE_LOOP_SPEED = false;
bool TRACE_DYNAMICS = false;


String ftos(float f) {
  char buffer[80];
  sprintf(buffer,"%.2f",f);
  return buffer;
}
