#include "logger.h"
#include <fstream>
#include "system.h"

using namespace std;

void log(string severity, string message)
{
  ofstream f;
  f.open("car_log.txt", ofstream::out | ofstream::app);
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
