#include "gtest/gtest.h"
#include <iostream>




#include "BeeperTest.h"
#include "BeepTest.h"
#include "FsmTest.h"
#include "PidTest.h"
#include "SequenceTest.h"
#include "StatisticsTest.h"

using namespace std;

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
