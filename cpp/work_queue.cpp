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
  bool ok = q.try_pop(s,500);
  if(ok) {
    cout << "failed, should have timed out" << endl;
  }
  else{
    cout << "passed, timed out as expected" << endl;
  }
}
