#ifndef WORK_QUEUE_H
#define WORK_QUEUE_H

#include <queue>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include "system.h"

using namespace std;
using namespace std::chrono;

struct StampedString {
  StampedString(const string & s, system_clock::time_point timestamp) : message(s), timestamp(timestamp) {
  }
  StampedString(){}
  
  string to_string() const;
  bool set_from_string(string s);

  string message;
  system_clock::time_point timestamp;
};

template <class T>
class WorkQueue {
  size_t max_size;
  std::mutex q_mutex;
  std::queue<T> q;
  std::condition_variable cv;

public:
  WorkQueue(size_t max_size = 10) : max_size(max_size) {
  }

  void push(T& s) {
    {
      std::lock_guard<std::mutex> lock(q_mutex);
      while(max_size && q.size() >= max_size) {
        q.pop();
      }
      q.emplace(s);
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
    if(q.empty()) {
      return false;
    }
    s = q.front();
    q.pop();

    return true;
  }
};


void test_work_queue();


#endif // WORK_QUEUE_H
