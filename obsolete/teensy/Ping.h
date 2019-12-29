#pragma once



// Interrupt service routines
void on_ping_reply_change();

class Ping {
public:
  int ping_pin, echo_pin;
  unsigned long ping_start_ms = 0;
  unsigned long ping_start_us = 0;
  bool _new_data_ready = false;
  bool _waiting_for_reply = false;

  double last_ping_distance_inches = 0.;

  unsigned long ping_rate_ms = 100;
  const unsigned long ping_timeout_us = 20000; // 20000 microseconds should be about 10 feet

  void init(int _ping_pin, int _echo_pin);
  bool new_data_ready();
  void set_distance_from_us(unsigned long us);
  void execute();
  double inches();
  int millimeters();
};
