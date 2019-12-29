#include "Statistics.h"


Statistics::Statistics() {
  reset();
}

void Statistics::reset() {
  sumx = sumxx = sumy = sumyy = sumxy = biasx = biasy = 0.0d;
  count = 0;
}

void Statistics::add(double x, double y) {

  // offset x to keep from overflowing
  if(count==0) {
    biasx = x;
    biasy = y;
  }
  x-=biasx;
  y-=biasy;
  ++count;
  sumx += x;
  sumxx += x*x;
  sumy += y;
  sumyy += y*y;
  sumxy += x*y;
}

double Statistics::slope() {
  return (sumxy - (sumx * sumy / count)) 
          / (sumxx - (sumx * sumx / count));
}
      
double Statistics::stdx() {
  return sqrt(sumxx/count - (sumx/count)*(sumx/count));
}

double Statistics::meany() {
  return sumy / count + biasy;
}
double Statistics::stdy() {
  return sqrt(sumyy/count - (sumy/count)*(sumx/count));
}

double Statistics::predicty(double x) {
  x -= biasx;
  return (sumy / count) - slope() * (sumx / count) + x * slope() + biasy;
}

double Statistics::correlation() {
  return (count * sumxy - sumx * sumy )
         / (sqrt(count*sumxx-sumx*sumx)*sqrt(count*sumyy-sumy*sumy));
}

