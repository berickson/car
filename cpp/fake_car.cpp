#include "fake_car.h"
#include <fstream>
#include <string>
#include "string_utils.h"
#include "logger.h"

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
    step();
    reset_odometry();
  } else {
    throw (string) "could not open " + recording_file_path;
  }
}


bool FakeCar::step() {
  string s;
  bool got_td = false;
  while(got_td == false) {
    if(getline(dynamics_file,s,'\n')) {
      got_td = process_line_from_log(s);
    } else {
      return false;
    }
  }
  return got_td;
}

void write_dynamics_csv_from_recording_file(string recording_path, string outpath ){
  fstream infile(recording_path);
  fstream csv;
  csv.open(outpath, ios_base::out);
  csv << Dynamics::csv_field_headers();
  string line;
  while(getline(infile, line)) {
    Dynamics d;
    if(Dynamics::from_log_string(d,line)) {
      csv << d.csv_fields() << endl;
    }
  }
}

void write_path_from_recording_file(string recording_path, string outpath) {
  log_info("entering write_path_from_recording_file");

  const double min_length = 0.03;
  FakeCar car(recording_path);
  fstream outfile;
  outfile.open(outpath, ios_base::out);

  string header = join({"secs","x","y","rear_x", "rear_y", "reverse", "heading","adj","esc","str","m/s"});
  outfile << header << endl;

  auto next = car.current_dynamics;
  auto current = car.current_dynamics;
  auto start = car.current_dynamics;

  Point p = car.get_front_position();
  bool reverse = false;
  bool next_reverse = false;
  unsigned i = 0;

  log_info("stepping through recording to create path");
  while(car.step()) {
    ++i;
    next = car.current_dynamics;
    Point p_next = car.get_front_position();
    int wheel_ticks = next.odometer_front_right - current.odometer_front_right;
    if (abs(wheel_ticks)>0 ){
      next_reverse = wheel_ticks < 0;
    } else {
      next_reverse = reverse;
    }

    // skip node if distance threshold hasn't been met
    if (distance(p,p_next) < min_length){
      continue;
    }

    reverse = next_reverse;

    double d = wheel_ticks * car.meters_per_odometer_tick;
    double wheel_meters_per_second = d / ((next.ms - current.ms) / 1000.);

    double seconds = (current.ms - start.ms)/1000.;
    Point rear_p = car.get_rear_position();
    current = next;

    outfile << seconds << ","
            << p.x << ","
            << p.y << ","
            << rear_p.x << ","
            << rear_p.y << ","
            << next_reverse << ","
            << car.get_heading().degrees() << ","
            << car.heading_adjustment.degrees() << ","
            << current.esc << ","
            << current.str << ","
            << wheel_meters_per_second << endl;
    p = p_next;
  }
  outfile.flush();
  outfile.close();
  log_info("entering write_path_from_recording_file");
}



#include "route.h"
void test_fake_car() {

  string recording_path = "/home/brian/Desktop/Dropbox/car/tracks/avc/routes/S/recording.csv";
  string route_path =     "../../cpp/test_data/2/test_route.csv";
  cout << "writing path " << route_path << " from recording " << recording_path << endl;
  write_path_from_recording_file(recording_path, route_path);
  cout << "done" << endl;
  cout << "verifying" << endl;
  Route r;
  r.load_from_file(route_path);
  cout << "max velocity before optimization " << r.get_max_velocity() << endl;
  r.optimize_velocity();
  cout << "max velocity after optimization " << r.get_max_velocity() << endl;
  cout << "optimize done" << endl;


  FakeCar car(recording_path);
  cout << "processing" << endl;
  int steps = 0;
  while(car.step()) {
    steps++;
  }
  cout << "processed " << steps << " steps to arrive at x:" << car.ackerman.x << " y: " << car.ackerman.y << endl;
}
