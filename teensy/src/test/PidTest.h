#include <iostream>
#include <math.h>

using namespace std;

#include "Pid.h"

#define str(s) #s
#define trace_eval(t) cout << str(t) << " = " << t << endl


void PidTest() {
  Pid pid;
  pid.set_sp(10.);
  pid.set_pv(0,0);
  cout << "default clamped output: " << pid.get_output() << endl;
  pid.set_min_max_output(-1000,1000);
  cout << "clamped to 1000 output: " << pid.get_output() << endl;
  pid.set_pv(5,1);
  cout << "output without kd: " << pid.get_output() << endl;
  pid.kd = 1;
  cout << "output with kd: " << pid.get_output() << endl;
  pid.ki = 0.1;
  cout << "output with ki: " << pid.get_output() << endl;
}
