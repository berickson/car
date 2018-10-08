#include <gtest/gtest.h>
//#include <gmock/gmock-matchers.h>
#include "../geometry.h"
#include "../string_utils.h"
#include <set>

using namespace testing;
using namespace std;

void test_acceleration_for_distance_and_velocities(double d, double v1, double v2) {
  cout << "from " << v1 << "m/s to " << v2 << "m/s in " << d
       <<  "m requires a of "
       <<  acceleration_for_distance_and_velocities(d, v1, v2)
       << "m/s^2" << endl;
}

TEST(geometry,geometry) {

  EXPECT_EQ(acceleration_for_distance_and_velocities(1,0,3),4.5);
  EXPECT_EQ(acceleration_for_distance_and_velocities(1,3,0),-4.5);
  EXPECT_EQ(acceleration_for_distance_and_velocities(2,0,3),2.25);
  EXPECT_EQ(acceleration_for_distance_and_velocities(1,-3,3),9);
}

template<class t> string to_string(vector<t> v) {
  if(v.size()==0) {
    return "";
  }
  string rv;
  rv += "[";
  rv += to_string(v[0]);
  for(int i = 1; i < v.size(); ++i) {
    rv += (string) ", " + to_string(v[i]);
  }
  rv += "]";
  return rv;
}

template <class T> 
vector<T> get_path_angles(const vector<T> & path_x, const vector<T> & path_y) {
  const int count = path_x.size();

  // sanity checks
  if(path_y.size() != count) {
    string e("path_x and path_y must be same size");
    cerr << e << endl;
    throw e;
  }

  vector<T> path_angles(count);

  for(int i = 0; i+1 < count; ++i) {
    path_angles[i] = atan2(path_y[i+1]-path_y[i], path_x[i+1]-path_x[i]);
  }
  if(count > 1) {
    path_angles[count-1] = path_angles[count-2];
  }
  return path_angles;
}

template<typename T> 
T cross_product_2d(T x1, T y1, T x2, T y2) {
  return x1*y2 - y1*x2;
}

/*
returns true if x, y is inside the closed, convex, counterclockwise
curve given by points in the lists obstacle_x, obstacle_y
*/
template <typename T>
bool is_inside_convex_shape (
    T x, 
    T y, 
    vector<T> obstacle_x, 
    vector<T> obstacle_y, 
    // distance inside, if negative, must be outside
    T distance_inside = 0 ) 
{
  int count = obstacle_x.size();
  if(obstacle_y.size() != count) {
    string e = "shape_x and shape_y must be same size";
    cerr << e << endl;
    throw(e);
  }
    // use cross products to ensure that point is to "left" of every line in obstacle
    for (int i = 1; i < count; ++i) {
      
      T x1 = obstacle_x[i-1];
      T x2 = obstacle_x[i];
      T y1 = obstacle_y[i-1];
      T y2 = obstacle_y[i];
      T v = cross_product_2d(x2-x1, y2-y1, x-x1, y-y1);

      if (v < distance_inside) {
        return false;
      }
    }
    return true;
}

template <typename T>
void transform_shape(const vector<T> & old_x, const vector<T> & old_y, T delta_x, T delta_y, T delta_theta, vector<T> & new_x, vector<T> & new_y) {
  int count = old_x.size();
  if(old_y.size() != count || new_x.size() != count || new_y.size() != count) {
    string s = "all vectors must be the same size";
    cerr << s << endl;
    throw s;
  }

  for (int i = 0; i < count; ++i) {
    T x = old_x[i];
    T y = old_y[i];

    T sin_theta = sin(delta_theta);
    T cos_theta = cos(delta_theta);

    new_x[i] = x * cos_theta - y* sin_theta + delta_x;
    new_y[i] = y * cos_theta + x* sin_theta + delta_y;
  }

}

template <typename T>
set<int> lidar_path_intersections(
    const vector<T> & path_x, 
    const vector<T> & path_y, 
    const vector<T> & path_theta, 
    const vector<T> & lidar_theta, 
    const vector<T> & lidar_l,
    const vector<T> & car_shape_x,
    const vector<T> & car_shape_y,
    T minimum_gap = 0.0) 
{
  int path_count = path_x.size();
  int lidar_count = lidar_theta.size();
  int car_shape_count = car_shape_x.size();

  set<int> lidar_collision_indexes;

  // sanity checks
  if(path_y.size() != path_count) {
    string s = "size of path_x and path_y must be the same";
    cerr << s << endl;
    throw s;
  }

  if(lidar_l.size() != lidar_count) {
    string s = "size of lidar_theta and lidar_l must be the same";
    cerr << s << endl;
    throw s;
  }

  if(car_shape_y.size() != car_shape_count) {
    string s = "size of car_shape_x and car_shape_y must be the same";
    cerr << s << endl;
    throw s;
  }


  // pre allocate corners
  vector<T> new_corners_x(car_shape_x.size());
  vector<T> new_corners_y(car_shape_y.size());

  for(int i = 0; i < path_count; ++i) {
    transform_shape(car_shape_x, car_shape_y, path_x[i], path_y[i], path_theta[i], new_corners_x, new_corners_y);
    for(int j = 0; j < lidar_count; ++j) {
      T lidar_x = lidar_l[j] * cos(lidar_theta[j]);
      T lidar_y = lidar_l[j] * sin(lidar_theta[j]);
      if(is_inside_convex_shape(lidar_x, lidar_y, new_corners_x, new_corners_y, -minimum_gap)) {
        lidar_collision_indexes.emplace(j);
      }
    }
  }
  return lidar_collision_indexes;
}


TEST(collision, get_path_angles) {
  vector<double> path_x{0,1,1};
  vector<double> path_y{0,0,1};
  vector<double> path_angles = get_path_angles(path_x,path_y);
  EXPECT_EQ(path_angles.size(), 3);
  EXPECT_EQ(path_angles[0], 0);
  EXPECT_EQ(path_angles[1], M_PI_2);
  EXPECT_EQ(path_angles[2], path_angles[1]);
}

template <class T>
void test_collisions() {
  // set up path path_x, path_y
  vector<T> path_x = linspace<T>(0,10,30);
  vector<T> path_y;
  path_y.reserve(path_x.size());
  for(auto x: path_x) {
    path_y.push_back(10*sin(x/3));
  }

  vector<T> lidar_theta = linspace<T>(0,359 * M_PI/ 180 ,72);

  // create a single lidar scan lidar_theta, lidar_l
  vector<T> lidar_l = linspace<T>(5,5,lidar_theta.size());

  // make the car shape car_corners_x, car_corners_y
  vector<T> car_corners_x{0,0,2,2,0};
  vector<T> car_corners_y{0.5,-0.5,-0.5,0.5,0.5};

  // calculate path angles
  vector<T> path_angles = get_path_angles(path_x, path_y);

  set<int> collisions = lidar_path_intersections(path_x, path_y, path_angles, lidar_theta, lidar_l, car_corners_x, car_corners_y);
  for(auto i : collisions) {
    cout << "collision detected at lidar" 
         << " index: " << i
         << " theta: " <<  Angle::radians(lidar_theta[i]).to_string() 
         << " lidar_l: " << lidar_l[i] 
         << " lidar_x: " << lidar_l[i] * cos(lidar_theta[i]) 
         << " lidar_y: " << lidar_l[i] * sin(lidar_theta[i]) 
         << endl;
  }

  EXPECT_EQ(collisions.size(),2);
  EXPECT_TRUE(collisions.count(14));
  EXPECT_TRUE(collisions.count(15));
}


TEST(collision,test_collision_float) {
  test_collisions<float>();
}

TEST(collision,test_collision_double) {
  test_collisions<double>();
}

TEST(collision,is_inside_convex_shape) {
  vector<float> shape_x{0,2,2,0,0};
  vector<float> shape_y{0,0,1,1,0};

  // simple in / out
  EXPECT_TRUE(is_inside_convex_shape<float>(1.9, 0.1, shape_x, shape_y));
  EXPECT_FALSE(is_inside_convex_shape<float>(0.5, 1.5, shape_x, shape_y));

  // inside and outside distance
  EXPECT_FALSE(is_inside_convex_shape<float>(1.9, 0.1, shape_x, shape_y, 0.2));
  EXPECT_TRUE(is_inside_convex_shape<float>(1.9, 0.1, shape_x, shape_y, -0.2));
  EXPECT_TRUE(is_inside_convex_shape<float>(2.1, -0.1, shape_x, shape_y, -0.2));
  EXPECT_FALSE(is_inside_convex_shape<float>(2.1, -0.1, shape_x, shape_y, 0.2));

}



