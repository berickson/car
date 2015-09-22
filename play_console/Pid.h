#pragma once
#include <math.h>

class Pid {
public:
  double kp = 1.0;
  double ki = 0.0;
  double kd = 0.0;
  double sp = 0.0;
  double sum_error = 0.0;

  double min_output = -1.;
  double max_output = 1.;

  double pv = NAN;
  double t = NAN;
  double prev_pv = NAN;
  double prev_t = NAN;

  void set_pv(double _pv, double _t);
  void set_min_max_output(double _min_output, double _max_output);
  void set_sp(double _sp);

  double get_output();
};
