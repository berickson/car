#include <iostream>
#include <math.h>

using namespace std;

class Pid {
public:
  double kp = 1.0;
  double ki = 0.0;
  double kd = 0.0;
  double setpoint = 0.0;
  double sum_error = 0.0;

  double min_output = -1.;
  double max_output = 1.;

  double input_value = NAN;
  double input_time = NAN;
  double prev_input_value = NAN;
  double prev_input_time = NAN;

  void set_input(double value, double t) {
    prev_input_value = input_value;
    prev_input_time = input_time;
    input_value = value;
    input_time = t;

    // integrate error
    if(!isnan(prev_input_time)) {
      sum_error += (input_time-prev_input_time) * (setpoint - input_value);
    }
  }

  void set_min_max_output(double _min_output, double _max_output) {
    min_output = _min_output;
    max_output = _max_output;
  }
  void set_setpoint(double value){
    setpoint = value;
    sum_error = 0;
  }

  double get_output(){
    // use the p term
    double output = (setpoint - input_value) * kp;

    // use the d term
    if(kd != 0.0 && !isnan(prev_input_value) && input_time > prev_input_time){

      double d = ((setpoint  - input_value) - (setpoint - prev_input_value)) / (input_time - prev_input_time);
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
  pid.set_setpoint(10.);
  pid.set_input(0,0);
  cout << "clamped output: " << pid.get_output() << endl;
  pid.set_min_max_output(-1000,1000);
  cout << "output: " << pid.get_output() << endl;
  pid.set_input(5,1);
  cout << "output without kd: " << pid.get_output() << endl;
  pid.kd = 1;
  cout << "output with kd: " << pid.get_output() << endl;
  pid.ki = 0.1;
  cout << "output with ki: " << pid.get_output() << endl;

  return 0;
}
