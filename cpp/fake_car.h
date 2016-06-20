#ifndef FAKECAR_H
#define FAKECAR_H

#include <fstream>
#include "car.h"

using namespace std;

class FakeCar : public Car{
public:
  FakeCar(string recording_path = "/home/brian/car/tracks/desk/routes/A/runs/47/recording.csv");
  void reset();
  bool step();

  string recording_file_path;
  fstream dynamics_file;

};

void write_path_from_recording_file(string recording_path, string outpath);

void test_fake_car();

#endif // FAKECAR_H
