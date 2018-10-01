#ifndef USB_H
#define USB_H

#include <list>
#include <thread>
#include <mutex>
#include <sstream>
#include "work_queue.h"

using namespace std;
using namespace std::chrono;

class Usb {
public:
  ~Usb();
  void run(string device_path);
  void stop();
  void write_line(string text);
  void add_line_listener(WorkQueue<StampedString>*);
  void remove_line_listener(WorkQueue<StampedString>*);
  void write_on_connect(string s);
  void flush();
  //std::string path ;
  
private:
  vector<char> read_buffer;
  string _device_path;
  string _write_on_connect = "\ntd+\n";
  string string_pending_write;
  std::mutex usb_mutex;
  bool running = false;
  bool quit = false;
  void monitor_incoming_data();
  void monitor_incoming_data_thread();
  void send_to_listeners(string s);
  void process_data(const char * data);
  string leftover_data;
  list<WorkQueue<StampedString>*> line_listeners;
  thread run_thread;

};

void test_usb();

#endif // USB_H
