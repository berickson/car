#include "logger.h"
#include <fstream>
#include "system.h"
#include <sstream>

using namespace std;

void log(string severity, string message)
{
  ofstream f;
  f.open("car_log.txt", ofstream::out | ofstream::app );
  f << time_string() << "," << severity << "," << message << endl;
}

void log_error(string message)
{
  static string severity = "ERROR";
  log(severity, message);
}


void log_warning(string message)
{
  static string severity = "WARNING";
  log(severity, message);
}

void log_info(string message)
{
  static string severity = "INFO";
  log(severity, message);
}

void log_trace(string message)
{
  static string severity = "TRACE";
  log(severity, message);
}

log_warning_if_duration_exceeded::log_warning_if_duration_exceeded(string label, chrono::duration<double> max_time)
{
  start_time = std::chrono::high_resolution_clock::now();
  this->label = label;
  this->max_time = max_time;
}

log_warning_if_duration_exceeded::~log_warning_if_duration_exceeded()
{
  std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - start_time;
  if(duration > max_time) {
    stringstream ss;
    ss << "time exceeded for " << label
       << ". expected less than " << max_time.count()
       << ", was " << duration.count();
    log_warning(ss.str());
  }
}

