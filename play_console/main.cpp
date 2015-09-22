#include <iostream>
#include <math.h>

using namespace std;

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

  void set_pv(double _pv, double _t) {
    prev_pv = pv;
    prev_t = t;
    pv = _pv;
    t = _t;

    // integrate error
    if(!isnan(prev_t)) {
      sum_error += (t-prev_t) * (sp - pv);
    }
  }

  void set_min_max_output(double _min_output, double _max_output) {
    min_output = _min_output;
    max_output = _max_output;
  }
  void set_sp(double _sp){
    sp = _sp;
    sum_error = 0;
  }

  double get_output(){
    // use the p term
    double output = (sp - pv) * kp;

    // use the d term
    if(kd != 0.0 && !isnan(prev_pv) && t > prev_t){

      double d = ((sp  - pv) - (sp - prev_pv)) / (t - prev_t);
      output += kd * d;
    }

    if(ki  != 0) {
      output += ki * sum_error;
    }

    // clamp
    if(output > max_output) {
      output = max_output;
    }
    if(output < min_output) {
      output = min_output;
    }

    return output;
  }
};

#define str(s) #s
#define trace_eval(t) cout << str(t) << " = " << t << endl


int main(int argc, char ** argv) {
  Pid pid;
  pid.set_sp(10.);
  pid.set_pv(0,0);
  cout << "clamped output: " << pid.get_output() << endl;
  pid.set_min_max_output(-1000,1000);
  cout << "output: " << pid.get_output() << endl;
  pid.set_pv(5,1);
  cout << "output without kd: " << pid.get_output() << endl;
  pid.kd = 1;
  cout << "output with kd: " << pid.get_output() << endl;
  pid.ki = 0.1;
  cout << "output with ki: " << pid.get_output() << endl;

  return 0;
}
