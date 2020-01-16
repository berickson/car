#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <set>
#include <sstream>
#include <string>
#include <vector>
#include "eigen3/Eigen/Dense"
#include "math.h"
#include "logger.h"
#include "string-utils.h"

using namespace std;

struct Point {
  Point(double x = 0, double y = 0);
  double x;
  double y;
  string to_string() const;
  Point operator-(Point p2);
};

string to_string(const Point &p);

struct Angle {
  static Angle degrees(double d);

  static Angle radians(double rad);

  double radians() const;
  double degrees() const;
  void set_degrees(double d);
  void set_radians(double theta);

  void standardize();

  const string to_string();

  bool operator==(Angle &rhs);
  operator double() { return theta; }
  Angle &operator/=(double d);
  Angle operator/(double d) const;
  Angle operator*(double d) const;
  Angle operator+(const Angle &rhs) const;
  Angle &operator+=(const Angle &rhs);
  Angle operator-();
  Angle operator-(const Angle &rhs) const;

 private:
  double theta;  // radians
};

struct Pose2d {
  Pose2d(Angle heading, Point position)
      : heading(heading), position(position) {}
  Angle heading;
  Point position;
};

struct Transform2d {
  Pose2d operator()(Pose2d &pose);
  Point operator()(Point &point);

  static Transform2d pose_to_world_transform(Pose2d pose);
  static Transform2d world_to_pose_transform(Pose2d pose);

 private:
  Eigen::Transform<double, 2, Eigen::Isometry> t;
  Angle rotation;
  float dx;
  float dy;
};

//
// Angular geometry
//

double degrees(double radians);
double radians(double degrees);

//#returns theta in range of [-pi,pi)
double standardized_radians(double theta);
double standardized_degrees(double theta);

// returns theta2-theta1 in range of [-180,180)
double degrees_diff(double theta1, double theta2);

// returns length of vector (x,y)
double length(double x, double y);

//
// General geometry
//

// returns solutions to ax^2+bx+c=0
vector<double> quadratic(double a, double b, double c);

// returns distane from (x1,y1) to (x2,y2)
double distance(double x1, double y1, double x2, double y2);
double distance(Point p1, Point p2);
double distance_from_segment_to_pointt(Point start, Point end, Point p);

//
// Kinematics
//

// acceleration to go from v1 to v2 in distance d
double acceleration_for_distance_and_velocities(double x, double v1, double v2);

// returns first t greater than zero that will reach position x
double time_at_position(double x, double a, double v0, double x0 = 0.);

// returns velocity at time t with initial velocity v0 and acceleration a
double velocity_at_time(double t, double a, double v0);

// returns velocity at position x with acceleration a, initial velocity v0 and
// initial position x0
double velocity_at_position(double x, double a, double v0, double x0 = 0);

// returns unit length vector in the direction of p
Point unit_vector(Point p);

// angle between two vectors
// based on http://stackoverflow.com/a/16544330/383967
Angle angle_between(double x1, double y1, double x2, double y2);
Angle angle_between(Point p1, Point p2);

// returns direction from p1 to p2
Angle angle_to(Point p1, Point p2);

// returns y for given x based on x1,y1,x2,y2
double interpolate(double x, double x1, double y1, double x2, double y2);

double clamp(double value, double min_value, double max_value);

template <typename T>
vector<T> linspace(T from, T to, int count) {
  vector<T> v(count);
  if (count == 0) {
    return v;
  }
  if (count == 0) {
    v[0] = from;
    return v;
  }
  T d = from;
  T step = (to - from) / (count - 1.);
  for (int i = 0; i < count; ++i) {
    v[i] = d;
    d += step;
  }
  return v;
}

template <class T>
vector<T> get_path_angles(const vector<T> &path_x, const vector<T> &path_y) {
  const size_t count = path_x.size();

  // sanity checks
  if (path_y.size() != count) {
    string e("path_x and path_y must be same size");
    throw e;
  }

  vector<T> path_angles(count);

  for (size_t i = 0; i + 1 < count; ++i) {
    path_angles[i] =
        atan2(path_y[i + 1] - path_y[i], path_x[i + 1] - path_x[i]);
  }
  if (count > 1) {
    path_angles[count - 1] = path_angles[count - 2];
  }
  return path_angles;
}

template <typename T>
T cross_product_2d(T x1, T y1, T x2, T y2) {
  return x1 * y2 - y1 * x2;
}

/*
returns true if x, y is inside the closed, convex, counterclockwise
curve given by points in the lists obstacle_x, obstacle_y
*/
template <typename T>
bool is_inside_convex_shape(T x, T y, const vector<T> &obstacle_x,
                            const vector<T> &obstacle_y,
                            // distance inside, if negative, must be outside
                            T distance_inside = 0) {
  size_t count = obstacle_x.size();
  if (obstacle_y.size() != count) {
    string e = "shape_x and shape_y must be same size";
    throw(e);
  }
  // use cross products to ensure that point is to "left" of every line in
  // obstacle
  for (size_t i = 1; i < count; ++i) {
    T x1 = obstacle_x[i - 1];
    T x2 = obstacle_x[i];
    T y1 = obstacle_y[i - 1];
    T y2 = obstacle_y[i];
    T v = cross_product_2d(x2 - x1, y2 - y1, x - x1, y - y1);

    if (v < distance_inside) {
      return false;
    }
  }
  return true;
}

template <typename T>
void transform_shape(const vector<T> &old_x, const vector<T> &old_y, T delta_x,
                     T delta_y, T delta_theta, vector<T> &new_x,
                     vector<T> &new_y) {
  size_t count = old_x.size();
  if (old_y.size() != count || new_x.size() != count || new_y.size() != count) {
    string s = "all vectors must be the same size";
    throw s;
  }

  T sin_theta = sin(delta_theta);
  T cos_theta = cos(delta_theta);

  for (size_t i = 0; i < count; ++i) {
    T x = old_x[i];
    T y = old_y[i];

    new_x[i] = x * cos_theta - y * sin_theta + delta_x;
    new_y[i] = y * cos_theta + x * sin_theta + delta_y;
  }
}

template <typename T>
vector<size_t> lidar_path_intersections(
    const vector<T> &path_x, const vector<T> &path_y,
    const vector<T> &path_theta, const vector<T> &lidar_x,
    const vector<T> &lidar_y, const vector<T> &car_shape_x,
    const vector<T> &car_shape_y, T minimum_gap = 0.0) {
  size_t path_count = path_x.size();
  size_t lidar_count = lidar_x.size();
  size_t car_shape_count = car_shape_x.size();

  vector<size_t> lidar_collision_indexes;

  // sanity checks
  if (path_y.size() != path_count) {
    string s = "size of path_x and path_y must be the same";
    throw s;
  }

  if (lidar_y.size() != lidar_count) {
    string s = "size of lidar_x and lidar_y must be the same";
    throw s;
  }

  if (car_shape_y.size() != car_shape_count) {
    string s = "size of car_shape_x and car_shape_y must be the same";
    throw s;
  }

  // pre allocate new shape
  vector<T> new_shape_x(car_shape_x.size());
  vector<T> new_shape_y(car_shape_y.size());

  for (size_t i = 0; i < path_count; ++i) {
    transform_shape(car_shape_x, car_shape_y, path_x[i], path_y[i],
                    path_theta[i], new_shape_x, new_shape_y);
    for (size_t j = 0; j < lidar_count; ++j) {
      if (is_inside_convex_shape(lidar_x[j], lidar_y[j], new_shape_x,
                                 new_shape_y, -minimum_gap)) {
        lidar_collision_indexes.emplace_back(i);
        log_info("colliding lidar at " +to_string(lidar_x[j]) +" " +to_string(lidar_y[j]));
        log_info("path i: " + to_string(i) + " x: "+ to_string(path_x[i]) + " y: " + to_string(path_y[i])+ " theta: " + to_string(path_theta[i]));
        log_info("new_shape_x: "+ vector_to_string(new_shape_x));
        log_info("new_shape_y: "+ vector_to_string(new_shape_y));
        break;
      }
    }
  }
  return lidar_collision_indexes;
}

void test_geometry();

#endif  // GEOMETRY_H
