#include <gtest/gtest.h>
//#include <gmock/gmock-matchers.h>
#include "../ackerman.h"


using namespace testing;

TEST(Ackerman, arc_to_relative) {
  // zero width makes it like a bicycle
  auto car = Ackerman(0, 1, Point(5,4));
  auto arc = car.arc_to_relative_location(50,40);
  auto str = Angle::radians(arc.steer_radians);
  car.move_right_wheel(str, arc.arc_len);
  ASSERT_FLOAT_EQ(car.front_left_position().x,55);
  ASSERT_FLOAT_EQ(car.front_left_position().y,44);
  cout << "arc len: " << arc.arc_len << endl;
  cout << "wheel angle: " << str.to_string() << endl;
}

