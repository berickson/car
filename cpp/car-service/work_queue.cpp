#include "work_queue.h"
#include "iostream"

using namespace std;

using namespace std::chrono_literals;
void test_work_queue() {
  WorkQueue<string> q;
  q.push("hello");
  string s;
  chrono::milliseconds time_out(500);
  if(q.try_pop(s, time_out)){
    cout << "got " << s << " from queue" << endl;
  }
  bool timed_out = q.try_pop(s,time_out);
  cout << "executed queue success of 0, was " << timed_out << endl;
}
