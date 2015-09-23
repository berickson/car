#include "Pid.h"

void Pid::set_pv(double _pv, double _t) {
  prev_pv = pv;
  prev_t = t;
  pv = _pv;
  t = _t;

  // integrate error
  if(!isnan(prev_t)) {
    sum_error += (t-prev_t) * (sp - pv);
  }
}

void Pid::set_min_max_output(double _min_output, double _max_output) {
  min_output = _min_output;
  max_output = _max_output;
}

void Pid::set_sp(double _sp){
  sp = _sp;
  sum_error = 0;
}

double Pid::get_output(){
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
