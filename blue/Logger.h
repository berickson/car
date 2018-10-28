#pragma once


extern bool LOG_ERROR;
extern bool LOG_INFO;
extern bool LOG_TRACE;
extern bool TRACE_RX;
extern bool TRACE_PINGS;
extern bool TRACE_MPU;
extern bool TRACE_LOOP_SPEED;
extern bool TRACE_TELEMETRY;
extern bool TD;
extern bool TD2;

#include "Arduino.h"

void log_line(String s);

#define log(__flag,__s)    \
if(__flag) { \
  String __text =(String) #__flag + "," + __s; \
  log_line(__text); \
}  


// returns float as string to overcome bug in Arduino casting
String ftos(float f, int n = 2);
