#include <gtest/gtest.h>
#include "../async_buf.h"

using namespace testing;
using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

TEST(async_buf, performance) {
  auto start = high_resolution_clock::now();
  const int num_lines = 1000000;
  duration<float> d1,d2;
  {
    async_buf buf("deleteme.txt");
    ostream f(&buf);
    for(int i = 0; i < num_lines; i++) {
      f << "this is line number" << i << "\n";
    }
    d1 = high_resolution_clock::now()-start;
  }
  d2 = high_resolution_clock::now()-start;
  cout << "time to write " << num_lines <<" was " << d1.count() << "seconds" << endl;
  cout << "time including flush was " << d2.count() << "seconds" << endl;
  std::remove("deleteme.txt");

}
