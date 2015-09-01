#include "math.h"
#include "stdint.h"


#include  "../libraries/MPU6050/helper_3dmath.h"

#include <iostream>

using namespace std;

#define str(s) #s
#define trace_eval(t) cout << str(t) << " = " << t << endl

ostream & operator << (ostream & s, const Quaternion & q_) {
  Quaternion & q = const_cast<Quaternion &>(q_);
  s << "w: " << q.w << " x: " << q.x << " y: " << q.y << " z: " << q.z;
}


double dot(Quaternion q1, Quaternion q2) {
  return q1.w*q2.w + q1.x*q2.x +q1.y*q2.y + q1.z*q2.z;
}

double angle(Quaternion q1, Quaternion q2) {
  return acos(2* (pow(dot(q1,q2),2.0)) - 1);
}

double degrees(double radians) {
  return radians * 180 / M_PI;
}

int main(int argc, char ** argv) {
  Quaternion q0(0.68,0.11,-0.72,0.06);
  Quaternion q1(0.53,-0.43,-0.59,-0.43);

  trace_eval(q0);
  trace_eval(q1);
  trace_eval(q0.getProduct(q1));
  trace_eval(q0.getProduct(q0.getConjugate()));
  trace_eval(q1.getProduct(q0.getConjugate()));
  trace_eval( dot(q0,q1) );
  trace_eval( angle(q0,q1));
  trace_eval( degrees(angle(q0,q1)));




  return 0;
}
