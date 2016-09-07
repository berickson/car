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

  bool queue_is_empty() {
    std::unique_lock<std::mutex> lock(q_mutex);
    return q.empty();
  }

  bool try_pop(T& s, int milliseconds) {

    while(queue_is_empty()) {
      std::unique_lock<std::mutex> lock(q_mutex);
      chrono::milliseconds timeout(milliseconds);
      if (cv.wait_for(lock, timeout) == std::cv_status::timeout ) {
        return false;
      }
    }
    {
      std::unique_lock<std::mutex> lock(q_mutex);
      s = q.front();
      q.pop();
    }

    return true;
  }
};


void test_work_queue();


#endif // WORK_QUEUE_H
