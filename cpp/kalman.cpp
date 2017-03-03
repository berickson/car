#include "kalman.h"

KalmanFilter::KalmanFilter() {
}

void KalmanFilter::init(float mean, float variance) {
  this->mean = mean;
  this->variance = variance;
}

void KalmanFilter::measure(float mean, float variance) {
  this->mean = (this->variance * mean + this->mean * variance) / (this->variance + variance);
  this->variance = 1 / ((1 / variance)+(1 / this->variance));
}

void KalmanFilter::update(float change, float variance) {
  this->mean += change;
  this->variance += variance;
}
