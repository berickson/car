#include "lidar.h"
#include <thread>
#include <chrono>
#include <sstream>
#include <vector>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/SVD>

std::__cxx11::string LidarMeasurement::display_string() {
  stringstream s;
  s << angle.to_string() << " ";
  if(status == measure_status::ok) {
    s << distance_meters << "m strength: " << signal_strength;
  }
  return s.str();
}

LidarScan::LidarScan() {
  measurements.resize(360);
}

std::__cxx11::string LidarScan::display_string() {
  stringstream s;
  for(LidarMeasurement & m : measurements) {
    s <<  m.display_string() << endl;
  }
  return s.str();
}

Eigen::Vector2f homogeneous_to_2d(const Eigen::Vector3f & v) {
  return {v[0]/v[2], v[1]/v[2]};
}

Eigen::Vector3f normalized(const Eigen::Vector3f & v) {
  Eigen::Vector3f rv;
  rv << v[0]/v[2],v[1]/v[2],1;
  return rv;
}

double distance_point_to_line(const Eigen::Vector3f & p, const Eigen::Vector3f & l) {

  return fabs(normalized(l).dot(normalized(p)));
}

/*
Finds the best fit line in the form ax + by + c = 0
points is matrix with homogeneous points in each row
returns a,b,c (which is homogeneous coordinates for the line)
*/
//template<typename Derived>
Eigen::Vector3f fit_line(const Eigen::MatrixX3f & points) {
  Eigen::JacobiSVD<Eigen::MatrixXf> svd(points,Eigen::ComputeFullV);
  const Eigen::MatrixX3f & V = svd.matrixV();
  cout << V.rows() << endl;
  Eigen::Vector3f rv = V.col(V.rows()-1);
  return rv;

}

// returns homogeneous coordinate to point on line closest to given point
// see https://math.stackexchange.com/questions/727743/homogeneous-coordinates
Eigen::Vector3f closest_point_on_line(Eigen::Vector3f line, Eigen::Vector3f point) {
  Eigen::Vector3f pn = normalized(point);
  float a = pn(0);
  float b = pn(1);
  float c = pn(2);

  float u = line(0);
  float v = line(1);
  float w = line(2);
  Eigen::Vector3f rv = normalized({b*(b*u-a*v)-a*c, -a*(b*u-a*v)-b*c, w*(a*a+b*b)});
  return rv;
}

/*
 takes a matrix of homogeneous points along the line

 Finds the best fit line in the form ax + by + c = 0
 returns a,b,c (which is homogeneous coordinates for the line
*/
template <typename Derived>
bool is_line(const Eigen::MatrixBase<Derived> & points, double tolerance) {
  if(points.hasNaN()){
    return false;
  }
  Eigen::Vector3f p1 = points.row(0);
  Eigen::Vector3f p2 = points.row(points.rows()-1);

  auto line = p1.cross(p2);
  for(int i = 0; i < points.rows(); i++) {
    Eigen::Vector3f p = points.row(i);
    double d = distance_point_to_line(p, line);
    if(d>tolerance)
      return false;
  }
  return true;
}

vector<LidarScan::ScanSegment> LidarScan::find_lines(double tolerance) {
  vector<ScanSegment> found_lines;
  Eigen::Matrix<float,360,3> points;

  // get all measurment locations as homogeneous 2d points
  for(int i=0; i < 360; ++i) {
    LidarMeasurement & m = measurements[i];
    // null yields null
    if(m.status == LidarMeasurement::measure_status::ok) {
      points(i, 0) = m.distance_meters * cos(m.angle.radians());
      points(i, 1) = m.distance_meters * sin(m.angle.radians());
      points(i, 2) = 1;
    } else {
      points(i, 0) = NAN;
      points(i, 1) = NAN;
      points(i, 2) = NAN;
    }
  }

  uint16_t start = 0;
  uint16_t line_end = 0;
  while (start < measurements.size()) {
    line_end = start;
    for(uint16_t end = start+5; end < 360; end++) {
      auto const & block = points.block(start, 0, end-start+1, 3);
      if(is_line(block, tolerance)){
        line_end = end;
      } else {
        break;
      }
    }
    if(line_end != start) {
      auto const & block = points.block(start, 0, line_end-start+1, 3);
      Eigen::Vector3f line = fit_line(block);

      ScanSegment s;
      s.begin_index = start;
      s.end_index = line_end;
      //s.p1 = homogeneous_to_2d(closest_point_on_line(line,block.row(0)));
      //s.p2 = homogeneous_to_2d(closest_point_on_line(line,block.row(line_end-start)));
      s.p1 = homogeneous_to_2d(block.row(0));
      s.p2 = homogeneous_to_2d(block.row(line_end-start));

      found_lines.push_back(s);
      start = line_end + 1;
    } else {
      ++start;
    }
  }
  return found_lines;
}

bool LidarUnit::try_get_scan(int ms_to_wait = 5000)
{
  string l;
  while(true) {
    if(! usb_queue.try_pop(l, ms_to_wait)) {
      return false;
    }
    trim(l);
    vector<string> fields = split(l);
    // see if this is an angle measurment
    if(fields.size() > 2 && fields[1] == "A") {
      LidarMeasurement m;
      int degrees = atoi(fields[2].c_str());
      m.angle.set_degrees(degrees);
      string v = fields[3];
      if(v=="S") {
        m.status = LidarMeasurement::measure_status::low_signal;
      } else if (v=="I") {
        m.status = LidarMeasurement::measure_status::invalid_data;
      } else if (v=="CRC") {
        m.status = LidarMeasurement::measure_status::crc_error;
      } else {
        m.distance_meters = atoi(v.c_str())/1000.;
        m.signal_strength = atoi(fields[4].c_str());
        m.status = LidarMeasurement::measure_status::ok;
      }
      next_scan.measurements[degrees] = m;
      if(degrees == 359) {
        swap(current_scan, next_scan);
        completed_scan_count++;
        return true;
      }
    }
  }
  return false;
}

string LidarUnit::get_scan_csv_header()
{
  std::stringstream s;
  s << "scan_number,degrees,distance_meters,signal_strength" << endl;
  return s.str();
}

string LidarUnit::get_scan_csv()
{
  stringstream s;
  for(int i = 0; i < 360; ++i) {
    LidarMeasurement & m = current_scan.measurements[i];
    s << completed_scan_count << "," << m.angle.degrees() << "," << m.distance_meters << "," << m.signal_strength << endl;
  }
  return s.str();
}

bool LidarUnit::get_scan() {
  try_get_scan();
  return true;
}

void LidarUnit::run() {
  usb2.write_on_connect("");
  usb2.run();
  usb2.write_line("ResetConfig");
  usb2.write_line("HideRaw");
  usb2.write_line("HideAll");
  usb2.write_line("SetSampleTime 40");
  usb2.write_line("ShowAll");
  //usb2.write_line("SetAngle 0, 15-30, 45-50, 10 ");
  usb2.write_line("ShowAll");
  usb2.write_line("MotorOn");

  usb2.add_line_listener(&usb_queue);
}

void LidarUnit::stop() {
  usb2.write_line("MotorOff");
  usb2.flush();
  usb2.stop();
}

void test_lidar() {
  cout << "lidar tests" << endl;
  Eigen::Matrix3f m;
  m << 0.,0.,1., 1.,1.,1., 2.,2.,1. ;
  cout << m << endl;
  cout << fit_line(m) << endl;
}
