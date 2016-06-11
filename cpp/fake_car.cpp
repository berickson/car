#include "fake_car.h"
#include <fstream>
#include <string>

using namespace std;
FakeCar::FakeCar(string recording_file_path):
  Car(online=false) {
  this->recording_file_path = recording_file_path;

  reset();
  step();
}

void FakeCar::reset() {
  if(dynamics_file.is_open()){
    dynamics_file.close();
  }
  dynamics_file.open(recording_file_path, ios_base::in);
  if(dynamics_file.is_open()) {
    reset_odometry();
    step();
  } else {
    throw (string) "could not open " + recording_file_path;
  }
}


bool FakeCar::step() {
  string s;
  if(getline(dynamics_file,s)) {
    process_line_from_log(s);
    return true;
  } else {
    return false;
  }
}


void test_fake_car() {
  string recording_path = "/home/brian/car/tracks/desk/routes/A/runs/47/recording.csv";

  FakeCar car(recording_path);
  cout << "processing" << endl;
  int steps = 0;
  while(car.step()) {
    steps++;
  }
  cout << "processed " << steps << " steps to arrive at x:" << car.ackerman.x << " y: " << car.ackerman.y << endl;
}
