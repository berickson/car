#ifndef PID_H
#define PID_H
#include "math.h"


class PID {
public:
  double k_p = 0;
  double k_i = 0;
  double k_d = 0;

  double last_e = NAN;
  double last_t = NAN;
  double sum_e = 0;
  double slope_e = 0;

  void add_reading(double t, double error);

  double output();
};

void test_pid();

#endif // PID_H
