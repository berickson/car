#ifndef WORK_QUEUE_H
#define WORK_QUEUE_H

#include <queue>
#include <mutex>
#include <chrono>
#include <condition_variable>

using namespace std;

class WorkQueue {
  std::mutex qMutex;
  std::queue<string> q;
  std::condition_variable populatedNotifier;

public:
  void push(std::string&& s) {
    {
      std::lock_guard<std::mutex> lock(qMutex);
      q.push(std::move(s));
    }

    populatedNotifier.notify_one();
  }

  bool try_pop(std::string& s, std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(qMutex);

    if(!populatedNotifier.wait_for(lock, timeout, [this] { return !q.empty(); }))
      return false;

    s = std::move(q.front());
    q.pop();

    return true;
  }
};

using namespace std::chrono_literals;
void test_work_queue() {
  WorkQueue q;
  q.push("hello");
  string s;
  chrono::milliseconds time_out(500);
  if(q.try_pop(s, time_out)){
    cout << "got " << s << " from queue" << endl;
  }
  bool timed_out = q.try_pop(s,time_out);
  cout << "executed queue success of 0, was " << timed_out << endl;
}


#endif // WORK_QUEUE_H
