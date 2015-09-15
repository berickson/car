#pragma once
#include "math.h"
#include  "helper_3dmath.h"

VectorFloat cross(VectorFloat a, VectorFloat b);

double dot(Quaternion q1, Quaternion q2);
double dot(VectorFloat u, VectorFloat v);
double angle(Quaternion q1, Quaternion q2);
double rads2degrees(double radians);
Quaternion diff(Quaternion q1, Quaternion q2);
// projects v onto plane with normal n
VectorFloat project(VectorFloat v, VectorFloat n);
// return angle quaternion q rotates about unit normal vector n
double normal_angle(Quaternion q, VectorFloat n);
