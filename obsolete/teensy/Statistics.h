#pragma once

#include "math.h"

class Statistics {
public:
  double sumx, sumxx, sumy, sumyy, sumxy,biasx,biasy;
  unsigned int count;
  
  Statistics();
  
  void reset();
  void add(double x, double y);
  double slope();
  double meany();
  double stdx();
  double stdy();
  double predicty(double x);
  double correlation();
};

