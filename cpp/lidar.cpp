#include "lidar.h"
#include <thread>
#include <chrono>
#include <sstream>
#include <vector>
#include "../../include/eigen3/Eigen/Dense"
#include "../../include/eigen3/Eigen/SVD"
#include "json.hpp"
#include "logger.h"
#include "string_utils.h"

using namespace Eigen;
Eigen::IOFormat HeavyFormat(FullPrecision, 0, ", ", ";\n", "[", "]", "[", "]");

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
  poses.resize(360);
  for(int i=0; i<360; ++i){
    measurements[i].angle.set_degrees(i);
    poses[i].theta = 0;
  }
}

std::__cxx11::string LidarScan::display_string() {
  stringstream s;
  for(LidarMeasurement & m : measurements) {
    s <<  m.display_string() << endl;
  }
  return s.str();
}

nlohmann::json LidarScan::get_json() {
  nlohmann::json scan_json;
  auto angles = nlohmann::json::array();
  auto distances = nlohmann::json::array();
  auto strengths = nlohmann::json::array();
  auto poses_x = nlohmann::json::array();
  auto poses_y = nlohmann::json::array();
  auto poses_theta = nlohmann::json::array();

  for(LidarMeasurement & m : measurements) {
      angles.push_back(m.angle.radians());
      distances.push_back(m.distance_meters);
      strengths.push_back(m.signal_strength);
  }
  for(Pose2dSimple & p : poses) {
      poses_x.push_back(p.x);
      poses_y.push_back(p.y);
      poses_theta.push_back(p.theta);
  }
  scan_json["number"] = scan_number;
  scan_json["angle"] = angles;
  scan_json["distance_meters"] = distances;
  scan_json["signal_strength"] = strengths;
  scan_json["pose_x"] = poses_x;
  scan_json["pose_y"] = poses_y;
  scan_json["pose_theta"] = poses_theta;
  return scan_json;
}


Vector2f homogeneous_to_2d(const Vector3f & v) {
  return {v[0]/v[2], v[1]/v[2]};
}

Vector3f normalized(const Vector3f & v) {
  Vector3f rv;
  rv << v[0]/v[2],v[1]/v[2],1;
  return rv;
}

double distance_point_to_line(const Vector3f & p, const Vector3f & l) {

  return fabs(normalized(l).dot(normalized(p)));
}

/*
Finds the best fit line in the form ax + by + c = 0
points is matrix with homogeneous points in each row
returns a,b,c (which is homogeneous coordinates for the line)
*/
//template<typename Derived>
Vector3f fit_line(const MatrixX3f & points) {
  JacobiSVD<MatrixXf> svd(points,ComputeFullV);
  const MatrixX3f & V = svd.matrixV();
  Vector3f rv = V.col(V.cols()-1);
  return rv;
}

// returns homogeneous coordinate to point on line closest to given point
// see https://math.stackexchange.com/questions/727743/homogeneous-coordinates

Vector3f closest_point_on_line(Vector3f line, Vector3f point) {
  Vector3f pn = normalized(point);
  float u = pn(0);
  float v = pn(1);
  float w = pn(2);

  float a = line(0);
  float b = line(1);
  float c = line(2);
  Vector3f rv = normalized({b*(b*u-a*v)-a*c, -a*(b*u-a*v)-b*c, w*(a*a+b*b)});
  return rv;
}


/*
 takes a matrix of homogeneous points along the line

 Finds the best fit line in the form ax + by + c = 0
 returns a,b,c (which is homogeneous coordinates for the line
*/
template <typename Derived>
bool is_line(const MatrixBase<Derived> & points, double tolerance) {
  if(points.hasNaN()){
    return false;
  }
  Vector3f p1 = points.row(0);
  Vector3f p2 = points.row(points.rows()-1);

  auto line = p1.cross(p2);
  for(int i = 0; i < points.rows(); i++) {
    Vector3f p = points.row(i);
    double d = distance_point_to_line(p, line);
    if(d>tolerance)
      return false;
  }
  return true;
}

vector<LidarScan::ScanSegment> LidarScan::find_lines(double tolerance, int min_point_count) {
  vector<ScanSegment> found_lines;
  Matrix<float,360,3> points;

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
    for(uint16_t end = start+min_point_count; end < 360; end++) {
      auto const & block = points.block(start, 0, end-start+1, 3);
      if(is_line(block, tolerance)){
        line_end = end;
      } else {
        break;
      }
    }
    if(line_end != start) {
      auto const & block = points.block(start, 0, line_end-start+1, 3);
      Vector3f line = fit_line(block);

      ScanSegment s;
      s.begin_index = start;
      s.end_index = line_end;
      s.p1 = homogeneous_to_2d(closest_point_on_line(line,block.row(0)));
      s.p2 = homogeneous_to_2d(closest_point_on_line(line,block.row(line_end-start)));
      //s.p1 = homogeneous_to_2d(block.row(0));
      //s.p2 = homogeneous_to_2d(block.row(line_end-start));

      found_lines.push_back(s);
      start = line_end + 1;
    } else {
      ++start;
    }
  }
  return found_lines;
}

Vector3f v2d_to_homogeneous(const Vector2f & x) {
  return {x(0),x(1),1};
}

Vector3f line_through_points(const Vector3f & p1, const Vector3f & p2) {
  return p1.cross(p2);
}

Vector3f line_through_points(const Vector2f & p1, const Vector2f & p2) {
  return line_through_points(v2d_to_homogeneous(p1), v2d_to_homogeneous(p2));
}

Vector3f line_intersection(Vector3f line1, Vector3f line2) {
  return line1.cross(line2);
};


// http://homepages.inf.ed.ac.uk/rbf/CVonline/LOCAL_COPIES/BEARDSLEY/node2.html
Angle angle_between_lines(Vector3f l1, Vector3f l2) {
  Vector3f l1n = normalized(l1);
  Vector3f l2n = normalized(l2);
  return Angle::radians(acos(l1n[0]*l2n[0] + l1n[1]*l2n[1]));
}

vector<Corner>  find_corners(const vector<LidarScan::ScanSegment> & walls) {
  vector<Corner> corners;
  for(unsigned int i = 1; i < walls.size(); ++i) {
    const LidarScan :: ScanSegment & w1 = walls[i-1];
    const LidarScan :: ScanSegment & w2 = walls[i];

    // corners must be near 90 degrees
    Vector3f line1 = line_through_points(w1.p1, w1.p2);
    Vector3f line2 = line_through_points(w2.p1, w2.p2);
    Angle theta = angle_between_lines(line1, line2);
    if(isnan(theta.radians())|| theta.degrees()>95 || theta.degrees() < 85) {
      //cout << "rejected corner " << theta.degrees() <<endl;
      continue;
    }


    Vector3f corner_point = line_intersection(line1, line2);

    // Adjacent line segments must be close to each other
    if((w1.p2 - w2.p1).norm() > std::min( (w1.p2-w1.p1).norm(), (w2.p2-w1.p1).norm())) {
      continue;
    }
    //cout << " found close corners at angle " << theta.degrees() << " degres" << endl;
    Corner corner;
    corner.p = homogeneous_to_2d(corner_point);
    corners.push_back(corner);

    // Angle must be greater than 45 degrees
    //if(abs(theta.degrees()<45))

  }
  return corners;
}

bool LidarUnit::try_get_scan(int ms_to_wait = 1)
{
  StampedString stamped_l;
  try {
  while(true) {
    if(! usb_queue.try_pop(stamped_l, ms_to_wait)) {
      return false;
    }
    stringstream ss(trimmed(stamped_l.message));
    string token;

    // first must be "A"
    if(!std::getline(ss,token,',')) break;
    if(token != "A") break;

    // angle
    LidarMeasurement m;
    if(!std::getline(ss,token,',')) throw string("error reading angle"); // todo: throw
    int degrees = atoi(token.c_str());
    m.angle.set_degrees(degrees);

    // status
    if(!std::getline(ss,token,',')) throw string("error reading status");
    if(token=="S") {
      m.status = LidarMeasurement::measure_status::low_signal;
    } else if (token=="I") {
      m.status = LidarMeasurement::measure_status::invalid_data;
    } else if (token=="CRC") {
      m.status = LidarMeasurement::measure_status::crc_error;
    } else {
      m.distance_meters = atoi(token.c_str())/1000.;
      m.status = LidarMeasurement::measure_status::ok;
      
      if(!std::getline(ss,token,',')) throw string("error reading signal strength");
      m.signal_strength = atoi(token.c_str());
    }
    next_scan.measurements[degrees] = m;
    next_scan.poses[degrees] = pose;

    if(degrees == 359) {
      swap(current_scan, next_scan);
      current_scan.scan_number = completed_scan_count;
      completed_scan_count++;
      return true;
    }
  }
  } catch (string error) {
    log_error("Exception caught in lidar.cpp: "+ error);
    log_error("processing message:" + stamped_l.message);
  } catch (...) {
    log_error("Exception caught in lidar.cpp");
    log_error("processing message:" + stamped_l.message);
  }
  return false;

    
/*    
    string & l = stamped_l.message;
    trim(l);
    vector<string> fields = split(l);
    // see if this is an angle measurment
    if(fields.size() == 4 && fields[0] == "A") {
      LidarMeasurement m;
      int degrees = atoi(fields[1].c_str());
      m.angle.set_degrees(degrees);
      string v = fields[2];
      if(v=="S") {
        m.status = LidarMeasurement::measure_status::low_signal;
      } else if (v=="I") {
        m.status = LidarMeasurement::measure_status::invalid_data;
      } else if (v=="CRC") {
        m.status = LidarMeasurement::measure_status::crc_error;
      } else {
        m.distance_meters = atoi(v.c_str())/1000.;
        m.signal_strength = atoi(fields[3].c_str());
        m.status = LidarMeasurement::measure_status::ok;
      }
      next_scan.measurements[degrees] = m;
      next_scan.poses[degrees] = pose;
      if(degrees == 359) {
        swap(current_scan, next_scan);
        current_scan.scan_number = completed_scan_count;
        completed_scan_count++;
        return true;
      }
    }
  }
  return false;
  */
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


void LidarUnit::set_pose(float x, float y, float theta) {
  pose.x = x;
  pose.y = y;
  pose.theta = theta;
}

// min: 180, max: 349
void LidarUnit::set_rpm(int rpm) {
  stringstream ss;
  ss << "SetRPM " << rpm;
  usb2.write_line(ss.str());
}


void LidarUnit::motor_on() {
  usb2.write_line("MotorOn");
  is_running = true;
}

void LidarUnit::motor_off() {
  usb2.write_line("MotorOff");
  is_running = false;
}


void LidarUnit::run() {
  usb2.write_on_connect(
    "ResetConfig\n"
    "HideRaw\n"
    "HideAll\n"
    "SetSampleTime 40\n"
    "ShowAll\n"
    "ShowAll\n"
    "MotorOn\n"
    "SetRPM 349"); // last \n added by usb2

  usb2.run("/dev/teensy12345");
  //usb2.write_line("ResetConfig");
  //usb2.write_line("HideRaw");
  //usb2.write_line("HideAll");
  //usb2.write_line("SetSampleTime 40");
  //usb2.write_line("ShowAll");
  ////usb2.write_line("SetAngle 0, 15-30, 45-50, 10 ");
  //usb2.write_line("ShowAll");
  //usb2.write_line("MotorOn");
  //set_rpm(349);
  is_running = true;

  usb2.add_line_listener(&usb_queue);
}

void LidarUnit::stop() {
  usb2.write_line("MotorOff");
  usb2.flush();
  usb2.stop();
}

void test_line_fit(MatrixX3f m, Vector3f p) {
  cout << "m" << m.format(HeavyFormat) << endl;
  cout << fit_line(m) << endl;
  cout << "nearest to " << endl;
  cout << p.format(HeavyFormat) << endl;
  cout << " is " << closest_point_on_line(fit_line(m), p).format(HeavyFormat) << endl;

}

void test_lidar() {
  cout << "lidar tests" << endl;

  Matrix<float,4,3> m;
  m << 1.,0.,1., 2.,1.,1., 3.,2.,1., 4.,3.,1. ;
  Vector3f p {3.0,0.1,1};
  test_line_fit(m,p);
  cout << "---------------------------" << endl;
  m << 0.,0.,1., 1.,0.,1., 2.,0.,1., 3.,0.,1. ;
  test_line_fit(m,p);

}
