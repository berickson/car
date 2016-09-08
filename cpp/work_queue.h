#ifndef WORK_QUEUE_H
#define WORK_QUEUE_H

#include <queue>
#include <mutex>
#include <chrono>
#include <condition_variable>

using namespace std;
using namespace std::chrono;
template <class T>
class WorkQueue {
  std::mutex q_mutex;
  std::queue<T> q;
  std::condition_variable cv;

public:
  void push(T& s) {
    {
      std::lock_guard<std::mutex> lock(q_mutex);
      q.push(s);
    }

    cv.notify_one();
  }

  bool try_pop(T& s, int milliseconds) {
    std::unique_lock<std::mutex> lock(q_mutex);
    if(q.empty()) {
      chrono::milliseconds timeout(milliseconds);
      if (!cv.wait_for(lock, timeout,[this](){return !q.empty(); }))
        return false;
    }
    s = q.front();
    q.pop();

    return true;
  }
};


void test_work_queue();


#endif // WORK_QUEUE_H
