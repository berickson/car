#include "pid.h"

// note: error is sp - pv
void PID::add_reading(double t, double error) {

  if(!isnan(last_t)) {
    double dt = t - last_t;
    if(!isnan(last_e) && dt > 0)
      slope_e = (error-last_e) / dt;
    sum_e += error * dt;
  }

  last_e = error;
  last_t = t;
}

double PID::output() {
  double rv = 0;

  // P
  if(!isnan(last_e))
    rv += k_p * last_e;

  // I
  rv += k_i * sum_e;

  // D
  if(!isnan(slope_e))
    rv += k_d * slope_e;

  return rv;
}

#include <iostream>

using namespace std;

void test_pid() {
  PID pid;
  pid.k_d = 1;
  cout << pid.output() << endl;
  pid.add_reading(1,1);
  cout << pid.output() << endl;
  pid.add_reading(2,2);
  cout << pid.output() << endl;
  pid.add_reading(3,0);
  cout << pid.output() << endl;
  pid.add_reading(4,0);
  cout << pid.output() << endl;
}
