#include "work_queue.h"
#include "iostream"
#include <string>

using namespace std;

void test_work_queue() {
  WorkQueue<string> q;
  string t = "hello";
  q.push(t);
  string s;
  if(q.try_pop(s, 500)){
    cout << "got " << s << " from queue" << endl;
  }
  bool timed_out = q.try_pop(s,500);
  cout << "executed queue success of 0, was " << timed_out << endl;
}
