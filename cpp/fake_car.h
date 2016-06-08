#ifndef FAKECAR_H
#define FAKECAR_H

#include <fstream>
#include "car.h"

using namespace std;

class FakeCar : public Car{
public:
  FakeCar(string recording_path);
  void reset();
  bool step();

  string recording_file_path;
  fstream dynamics_file;

};

void test_fake_car();

#endif // FAKECAR_H
