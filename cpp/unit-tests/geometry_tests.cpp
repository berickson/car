#include <gtest/gtest.h>
//#include <gmock/gmock-matchers.h>
#include <set>
#include "../geometry.h"
#include "../string-utils.h"

using namespace testing;
using namespace std;

void test_acceleration_for_distance_and_velocities(
  double d, double v1,double v2) {
  cout << "from " << v1 << "m/s to " << v2 << "m/s in " << d
       << "m requires a of "
       << acceleration_for_distance_and_velocities(d, v1, v2) << "m/s^2"
       << endl;
}

TEST(geometry, geometry) {
  EXPECT_EQ(acceleration_for_distance_and_velocities(1, 0, 3), 4.5);
  EXPECT_EQ(acceleration_for_distance_and_velocities(1, 3, 0), -4.5);
  EXPECT_EQ(acceleration_for_distance_and_velocities(2, 0, 3), 2.25);
  EXPECT_EQ(acceleration_for_distance_and_velocities(1, -3, 3), 9);
}

template <class T>
string to_string(const vector<T> &v) {
  if (v.size() == 0) {
    return "";
  }
  string rv;
  rv += "[";
  rv += to_string(v[0]);
  for (int i = 1; i < v.size(); ++i) {
    rv += (string) ", " + to_string(v[i]);
  }
  rv += "]";
  return rv;
}

template <class T>
void expect_vector_eq(vector<T> v1, vector<T> v2) {
  EXPECT_EQ(v1.size(), v2.size());
  for(size_t i=0; i<v1.size(); ++i) {
    EXPECT_NEAR(v1[i],v2[i],1E-6);
  }
}

TEST(collision, get_path_angles) {
  vector<double> path_x{0, 1, 1};
  vector<double> path_y{0, 0, 1};
  vector<double> path_angles = get_path_angles(path_x, path_y);
  EXPECT_EQ(path_angles.size(), 3);
  EXPECT_EQ(path_angles[0], 0);
  EXPECT_EQ(path_angles[1], M_PI_2);
  EXPECT_EQ(path_angles[2], path_angles[1]);
}

template <class T>
void test_collisions(int repeat = 1) {
  cout << "0" << endl;
  // set up path path_x, path_y
  vector<T> path_x = linspace<T>(0, 10, 30);
  vector<T> path_y;
  path_y.reserve(path_x.size());
  for (auto x : path_x) {
    path_y.push_back(10 * sin(x / 3));
  }

  vector<T> lidar_theta = linspace<T>(0, 359 * M_PI / 180, 72);

  // create a single lidar scan lidar_theta, lidar_l
  vector<T> lidar_l = linspace<T>(5, 5, lidar_theta.size());

  // make the car shape car_shape_x, car_shape_y
  vector<T> car_shape_x{0, 0, 2, 2, 0};
  vector<T> car_shape_y{0.5, -0.5, -0.5, 0.5, 0.5};

  // calculate path angles
  vector<T> path_angles = get_path_angles(path_x, path_y);
  vector<T> lidar_x;
  vector<T> lidar_y;
  for(size_t i=0;i<lidar_theta.size();++i) {
    auto l=lidar_l[i];
    auto theta = lidar_theta[i];
    lidar_x.push_back(l*cos(theta));
    lidar_y.push_back(l*sin(theta));
  }

  vector<size_t> collisions;
  for (int i = 0; i < repeat; ++i) {
    try {
      collisions =
          lidar_path_intersections(path_x, path_y, path_angles, lidar_x,
                                  lidar_y, car_shape_x, car_shape_y);
    }
    catch(string s) {
      cout << "caught string: " << s << endl;

    }
  }

  expect_vector_eq(collisions, vector<size_t>({3, 4}));
}

TEST(collision, test_collision_float) { test_collisions<float>(); }

TEST(collision, test_collision_float_1000) { test_collisions<float>(1000); }

TEST(collision, test_collision_double_1000) { test_collisions<double>(1000); }

TEST(collision, test_collision_double) { test_collisions<double>(); }

TEST(collision, is_inside_convex_shape) {
  vector<float> shape_x{0, 2, 2, 0, 0};
  vector<float> shape_y{0, 0, 1, 1, 0};

  // simple in / out
  EXPECT_TRUE(is_inside_convex_shape<float>(1.9, 0.1, shape_x, shape_y));
  EXPECT_FALSE(is_inside_convex_shape<float>(0.5, 1.5, shape_x, shape_y));

  // inside and outside distance
  EXPECT_FALSE(is_inside_convex_shape<float>(1.9, 0.1, shape_x, shape_y, 0.2));
  EXPECT_TRUE(is_inside_convex_shape<float>(1.9, 0.1, shape_x, shape_y, -0.2));
  EXPECT_TRUE(is_inside_convex_shape<float>(2.1, -0.1, shape_x, shape_y, -0.2));
  EXPECT_FALSE(is_inside_convex_shape<float>(2.1, -0.1, shape_x, shape_y, 0.2));
}

TEST(linspace, linspace) {
  auto v0 = linspace<float>(2, 3, 0);
  EXPECT_EQ(v0.size(), 0);

  auto v1 = linspace<float>(2, 3, 1);
  EXPECT_EQ(v1.size(), 1);
  EXPECT_FLOAT_EQ(v1[0], 2);

  auto v2 = linspace<float>(2, 3, 3);
  EXPECT_EQ(v2.size(), 3);
  EXPECT_FLOAT_EQ(v2[0], 2);
  EXPECT_FLOAT_EQ(v2[1], 2.5);
  EXPECT_FLOAT_EQ(v2[2], 3);
}

TEST(Pose2d, construction) {
  Pose2d p(Angle::degrees(90), Point(2,1));
  EXPECT_FLOAT_EQ(p.heading.degrees(), 90);
  EXPECT_FLOAT_EQ(p.position.x, 2);
  EXPECT_FLOAT_EQ(p.position.y, 1);
}

TEST(Transform2d, world_to_pose) {
  Pose2d pose(Angle::degrees(90), Point(5, 3));

  Pose2d world_p(Angle::degrees(10), Point(1, 2));

  Transform2d world_to_pose = Transform2d::world_to_pose_transform(pose);
  Pose2d pose_p = world_to_pose(world_p);
  EXPECT_FLOAT_EQ(pose_p.heading.degrees(), -80);
  EXPECT_FLOAT_EQ(pose_p.position.x, -1);
  EXPECT_FLOAT_EQ(pose_p.position.y, 4);

  Transform2d pose_to_world = Transform2d::pose_to_world_transform(pose);
  auto world_p2 = pose_to_world(pose_p);

  EXPECT_FLOAT_EQ(world_p2.position.x, world_p.position.x);
  EXPECT_FLOAT_EQ(world_p2.position.y, world_p.position.y);
  EXPECT_FLOAT_EQ(world_p2.heading.degrees(), world_p.heading.degrees());

  // origin in pose frame should be the same as pose in world frame
  Pose2d origin(Angle::radians(0), Point(0, 0));
  Pose2d pose_2 = pose_to_world(origin);
  EXPECT_FLOAT_EQ(pose_2.heading.degrees(), pose.heading.degrees());
  EXPECT_FLOAT_EQ(pose_2.position.x, pose.position.x);
  EXPECT_FLOAT_EQ(pose_2.position.y, pose.position.y);

  // pose in world frame should be the same as origin in pose frame
  Pose2d origin_2 = world_to_pose(pose);
  EXPECT_FLOAT_EQ(origin_2.heading.degrees(), 0);
  EXPECT_FLOAT_EQ(origin_2.position.x, 0);
  EXPECT_FLOAT_EQ(origin_2.position.y, 0);
}


TEST(geometry, transform_shape) {
  double l = 0.5;
  double w = 0.2;
  vector<double> shape_x{0, 0, l, l, 0};
  vector<double> shape_y{w/2,-w/2,-w/2,w/2,w/2};
  vector<double> new_shape_x(shape_x.size());
  vector<double> new_shape_y(shape_x.size());

  double dx,dy,dtheta;

  dx=0;dy=0;dtheta=0;
  transform_shape(shape_x, shape_y, dx, dy, dtheta, new_shape_x, new_shape_y);
  expect_vector_eq(new_shape_x, shape_x);
  expect_vector_eq(new_shape_y, shape_y);

  dx=3;dy=1;dtheta=0;
  transform_shape(shape_x, shape_y, dx, dy, dtheta, new_shape_x, new_shape_y);
  expect_vector_eq(new_shape_x, {0+dx, 0+dx, l+dx, l+dx, 0+dx});
  expect_vector_eq(new_shape_y, {w/2+dy,-w/2+dy,-w/2+dy,w/2+dy,w/2+dy});

  dx=0;dy=0;dtheta=M_PI_2;
  transform_shape(shape_x, shape_y, dx, dy, dtheta, new_shape_x, new_shape_y);
  expect_vector_eq(new_shape_y, {0, 0, l, l, 0});
  expect_vector_eq(new_shape_x, {-w/2,w/2,w/2,-w/2,-w/2});

  dx=1;dy=2;dtheta=M_PI_2;
  transform_shape(shape_x, shape_y, dx, dy, dtheta, new_shape_x, new_shape_y);
  expect_vector_eq(new_shape_y, {0+dy, 0+dy, l+dy, l+dy, 0+dy});
  expect_vector_eq(new_shape_x, {-w/2+dx,w/2+dx,w/2+dx,-w/2+dx,-w/2+dx});
}