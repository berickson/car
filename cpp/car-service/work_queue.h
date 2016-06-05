#ifndef WORK_QUEUE_H
#define WORK_QUEUE_H

#include <queue>
#include <mutex>
#include <chrono>
#include <condition_variable>

using namespace std;
template <class T>
class WorkQueue {
  std::mutex qMutex;
  std::queue<T> q;
  std::condition_variable populatedNotifier;

public:
  void push(T&& s) {
    {
      std::lock_guard<std::mutex> lock(qMutex);
      q.push(std::move(s));
    }

    populatedNotifier.notify_one();
  }

  bool try_pop(T& s, std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(qMutex);

    if(!populatedNotifier.wait_for(lock, timeout, [this] { return !q.empty(); }))
      return false;

    s = std::move(q.front());
    q.pop();

    return true;
  }
};

using namespace std::chrono_literals;
void test_work_queue();


#endif // WORK_QUEUE_H
