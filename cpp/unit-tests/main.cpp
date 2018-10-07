#include <gtest/gtest.h>

int run(int argc, char *argv[])
{
  {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
  }
}


int main(int argc, char *argv[])
{
  run(argc, argv);
  std::cout << "done" << std::endl;
}
