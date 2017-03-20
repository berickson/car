#include "../work_queue.h"
#include "iostream"
#include <string>
#include <sstream>
#include <thread>
#include <chrono>

using namespace std;
#include <gtest/gtest.h>

using namespace testing;

TEST(WorkQueue, force_time_out) {
  WorkQueue<string> q;
  string t = "hello";
  q.push(t);
  string s;
  EXPECT_TRUE(q.try_pop(s,500));
  EXPECT_FALSE(q.try_pop(s, 500));
}



void consumer(WorkQueue<string> * queue, int service_time, int expected_message_count) {
  int messages_received = 0;
  string s;
  while(queue->try_pop(s, 10)) {
    messages_received++;
    std::this_thread::sleep_for(1ms*service_time);
  }
  cout << "thread with service time " << service_time << "ms received " << messages_received << " messages" << endl;
  EXPECT_EQ(messages_received, expected_message_count);
}

TEST(WorkQueue, fast_and_slow_consumers) {
  auto start = std::chrono::high_resolution_clock::now();
  const int num_threads = 5;
  std::thread t[num_threads];
  WorkQueue<string> work_queues[num_threads];
  int message_count = 50;
  for(int i = 0; i < num_threads; i++) {
    t[i] = std::thread(consumer, &work_queues[i], (1+i), message_count);
  }
  for(int i = 0; i < message_count; i++) {
    stringstream ss;
    ss << "this is string " << i;
    string s = ss.str();
    for(int j = 0; j < num_threads; j++) {
      work_queues[j].push(s);
      std::this_thread::sleep_for(1ms);
    }
  }
  for (int i = 0; i < num_threads; ++i) {
    t[i].join();
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end-start;
  cout << "test done in " << elapsed.count() << "s" << endl;

}

TEST(WorkQueue, as_fast_as_possible) {
  int message_count = 1000000; // one million
  WorkQueue<string> queue;
  std::thread t(consumer, &queue, 0, message_count);
  string s = "hello";
  for(int i=0; i < message_count; i++) {
    queue.push(s);
  }
  t.join();
}
