#include "math.h"
#include "stdint.h"

#include "../car_control/vector_math.h"

#include <iostream>

using namespace std;

#define str(s) #s
#define trace_eval(t) cout << str(t) << " = " << t << endl

ostream & operator << (ostream & s, const Quaternion & q_) {
  Quaternion & q = const_cast<Quaternion &>(q_);
  s << "qw: " << q.w << " x: " << q.x << " y: " << q.y << " z: " << q.z;
}


int main(int argc, char ** argv) {
  Quaternion q0(0.69,0.11,-0.72,0.06);
  Quaternion q1(0.53,-0.43,-0.59,-0.43);

  trace_eval(q0);
  trace_eval(q1);
  trace_eval(q0.getProduct(q1));
  trace_eval(q0.getProduct(q0.getConjugate()));
  trace_eval(q1.getProduct(q0.getConjugate()));
  trace_eval( dot(q0,q1) );
  trace_eval( angle(q0,q1));
  trace_eval( degrees(angle(q0,q1)));
  trace_eval( degrees(normal_angle(diff(q1,q0),VectorFloat(1,0,0))));

  return 0;
}
