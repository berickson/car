#include "work_queue.h"
#include "iostream"

using namespace std;

void test_work_queue() {
  WorkQueue<string> q;
  q.push("hello");
  string s;
  if(q.try_pop(s, 500)){
    cout << "got " << s << " from queue" << endl;
  }
  bool timed_out = q.try_pop(s,500);
  cout << "executed queue success of 0, was " << timed_out << endl;
}
