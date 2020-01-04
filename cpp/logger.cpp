#include "logger.h"
#include <fstream>
#include "system.h"
#include <sstream>
#include <execinfo.h> // backtrace

#include <iostream>

using namespace std;

void log(string severity, string message)
{
  ofstream f;
  f.open("car-log.txt", ofstream::out | ofstream::app );
  f << time_string() << "," << severity << "," << message << endl;
  cout << time_string() << "," << severity << "," << message << endl;
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


log_entry_exit::log_entry_exit(string scope_label)
{
  this->scope_label = scope_label;
  log_info("entering " + scope_label);
}

log_entry_exit::~log_entry_exit()
{
  log_info("exiting " + scope_label);
}


void log_backtrace() {
  size_t max_count = 25;
  void * array[max_count];

  // get void*'s for all entries on the stack
  size_t count = backtrace(array, max_count);
  char **strings = backtrace_symbols(array, count);

  // print out all the frames to stderr
  for(int i = 0; i < count; ++i) {
    log_error(strings[i]);
  }
  free(strings);
}

void throw_and_log(string error) {
  log_error((string)"Throwing: " + error);
  log_backtrace();
}