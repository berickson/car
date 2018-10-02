#include <chrono>
#include <string>
using namespace std;
using namespace std::chrono;

struct PerformanceData {
  PerformanceData(const char * name);
  system_clock::time_point _construct_time;
  string name = "unnamed";
  long call_count = 0;
  long report_every_n_calls = 1000;
  system_clock::duration total_duration;
};

class MethodTracker {
public:
  PerformanceData &_data;
  system_clock::time_point _start_time;

  MethodTracker(PerformanceData & data);

  ~MethodTracker();
};
