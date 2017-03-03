#ifndef KALMAN_H
#define KALMAN_H

#include "math.h"

class KalmanFilter {
public:
    KalmanFilter();

    void init(float mean, float variance);
    void measure(float mean, float variance);
    void update(float change, float variance);

    float mean = 0;
    float variance = 1000;
};



#endif // KALMAN_H
