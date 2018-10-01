#include "work_queue.h"
#include "iostream"
#include <string>

using namespace std;


  
string StampedString::to_string() const{
  return (string) time_string(timestamp)+","+message;
}

bool StampedString::set_from_string(string s) {
  size_t i = s.find(",");
  if(i != 24) {
    return false;
  }
  string time_string = s.substr(0,i);
  timestamp = time_from_string(time_string);
  message = s.substr(i+1);
  return true;
}

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
