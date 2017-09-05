#include "kalman.h"

KalmanFilter::KalmanFilter() {
}

void KalmanFilter::init(float _mean, float _variance) {
  this->mean = _mean;
  this->variance = _variance;
}

void KalmanFilter::update(float _mean, float _variance) {
  this->mean = (this->variance * _mean + this->mean * _variance) / (this->variance + _variance);
  this->variance = 1. / ((1. / _variance)+(1. / this->variance));
}

void KalmanFilter::predict(float change, float _variance) {
  this->mean += change;
  this->variance += _variance;
}
