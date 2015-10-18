#include "gtest/gtest.h"
#include <iostream>




#include "BeeperTest.h"
#include "BeepTest.h"
#include "FsmTest.h"
#include "PidTest.h"
#include "SequenceTest.h"
#include "StatisticsTest.h"

using namespace std;
TEST(Statistics, unity_slope) {
  Statistics s;
  s.add(3,3);
  s.add(5,5);
  EXPECT_EQ(s.slope(),1) << "two points with slope of one";
  cout << "the slope was " << s.slope() << endl;
}



TEST(ListenersTest, LeaksWater) {
  bool x = true;
  EXPECT_TRUE(x);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
