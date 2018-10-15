#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <chrono>




void log_error(std::string);
void log_warning(std::string);
void log_info(std::string);
void log_trace(std::string);

// logs a warning if instance lives longer than max_time
class log_warning_if_duration_exceeded {
public:
  log_warning_if_duration_exceeded(std::string label, std::chrono::duration<double> max_time);
  ~log_warning_if_duration_exceeded();
private:
  std::string label;
  std::chrono::duration<double> max_time;
  std::chrono::high_resolution_clock::time_point start_time;
};

// logs info on entry and exit of a scope
class log_entry_exit {
public:
  log_entry_exit(std::string scope_label);
  ~log_entry_exit();
private:
  std::string scope_label;
};

void log_backtrace();
void throw_and_log(std::string message);


#endif // LOGGER_H
