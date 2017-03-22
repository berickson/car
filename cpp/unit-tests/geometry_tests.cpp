#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "../geometry.h"

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


