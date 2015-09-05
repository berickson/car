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




VectorFloat cross(VectorFloat a, VectorFloat b) {
    return VectorFloat(
        a.y*b.z-a.z*b.y, // x
        a.z*b.x-a.x*b.z, // y
        a.x*b.y-a.y*b.x  // z
    );
}

double dot(Quaternion q1, Quaternion q2) {
  return q1.w*q2.w + q1.x*q2.x +q1.y*q2.y + q1.z*q2.z;
}

double dot(VectorFloat u, VectorFloat v) {
    return u.x * v.x + u.y * v.y + u.z* v.z;
}

double angle(Quaternion q1, Quaternion q2) {
  return acos(2* (pow(dot(q1,q2),2.0)) - 1);
}

double degrees(double radians) {
  return radians * 180 / M_PI;
}

Quaternion diff(Quaternion q1, Quaternion q2){
    return q1.getConjugate().getNormalized().getProduct(q2);
}

// projects v onto plane with normal n
VectorFloat project(VectorFloat v, VectorFloat n) {
    double d = dot(v,n) / n.getMagnitude();
    return VectorFloat(v.x-d*v.x,v.y-d*v.y,v.z-d*v.z);
}

// return angle quaternion q rotates about unit normal vector n
double normal_angle(Quaternion q, VectorFloat n) {
    // find unit vector normal to n to make our x axis
    // todo: make sure it isn't parallel
    VectorFloat vx = cross(VectorFloat(1,1,1),n).getNormalized();

    // find unit vector orthogonal to vx and n to make our y axis
    VectorFloat vy = cross(n,vx).getNormalized();

    // rotate it and project onto plane
    VectorFloat rotx = project(vx.getRotated(&q),n);

    // express in terms of vx,vy
    VectorFloat p = VectorFloat(dot(vx,rotx), dot(vy,rotx), 0).getNormalized();

    return atan2(p.y,p.x);
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
  trace_eval( degrees(normal_angle(diff(q1,q0),VectorFloat(1,0,0))));

  return 0;
}
