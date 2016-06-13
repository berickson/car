#ifndef USB_H
#define USB_H

#include <list>
#include "work_queue.h"
#include <thread>
#include <mutex>

class Usb {
public:
  ~Usb();
  void run();
  void stop();
  void write_line(string text);
  void add_line_listener(WorkQueue<string>*);
  void remove_line_listener(WorkQueue<string>*);
  void write_on_connect(string s);
private:
  string _write_on_connect;
  string pending_write;
  std::mutex usb_mutex;
  bool running = false;
  bool quit = false;
  void monitor_incoming_data();
  void send_to_listeners(string s);
  void process_data(string data);
  string leftover_data;
  list<WorkQueue<string>*> line_listeners;
  thread run_thread;

};

void test_usb();

#endif // USB_H
