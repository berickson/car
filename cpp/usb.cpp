#include "usb.h"
#include <sys/stat.h>
#include <fcntl.h> // for open,close
#include <unistd.h> // for file operations, usleep
#include <iostream>
#include <termios.h> // for make_raw
#include <string>
#include <thread>
#include "ends_with.h"
#include "split.h"
#include "system.h"
#include "logger.h"

using namespace std;
using namespace std::chrono;

struct PerformanceData {
  PerformanceData(const char * name) : name(name) {
    _construct_time = system_clock::now();
  }
  system_clock::time_point _construct_time;
  std::string name = "unnamed";
  long call_count = 0;
  long report_every_n_calls = 1000;
  system_clock::duration total_duration;
};

class MethodTracker {
public:
  PerformanceData &_data;
  system_clock::time_point _start_time;

  MethodTracker(PerformanceData & data) : _data(data) {
    _start_time = system_clock::now();
  }

  ~MethodTracker() {
    auto end_time = system_clock::now();
    _data.total_duration += (end_time-_start_time);
    ++(_data.call_count);
    if((_data.call_count % _data.report_every_n_calls) == 0) {
      auto clock_elapsed = system_clock::now() - _data._construct_time;
      float percent_wall = 100. * _data.total_duration.count() / clock_elapsed.count();
      log_info(
        _data.name 
        + " call_count: " 
        + to_string(_data.call_count) 
        + " total_duration: " 
        + to_string(_data.total_duration.count() / 1E6) + " ms"
        + " average_duratioN: " 
        + to_string((_data.total_duration.count() / 1E6) / _data.call_count)+ " ms"
        + "% wall: " + to_string(percent_wall)
      );
    }
  }
};


Usb::~Usb() {
  stop();
}

void Usb::write_on_connect(string s) {
  _write_on_connect = s;
}

void Usb::add_line_listener(WorkQueue<StampedString>*listener){
  line_listeners.push_back(listener);
}

void Usb::remove_line_listener(WorkQueue<StampedString>*listener){
  line_listeners.remove(listener);
}

void Usb::send_to_listeners(string s) {
  // 9/30 - about 8.0%  clock wall time
  //      - time_string to int - about 2.6 % wall time
  //static PerformanceData perf_data("Usb::send_to_listeners");
  //MethodTracker t(perf_data);
  static long count = 0;

  StampedString payload(s, system_clock::now());


  //string stamped = to_string(++count)+","+s;
  for(auto listener : line_listeners) {
      // 9/30 - over 1.0%  clock wall time
      //static PerformanceData perf_data("Usb::send_to_listeners.push");
      //MethodTracker t(perf_data);
      listener->push(payload);
  }
}

bool wait_for_data(int fd, __time_t tv_sec = 1, __suseconds_t tv_usec = 0) {
  struct timeval timeout;
  /* Initialize the file descriptor set. */
  fd_set read_fds;
  FD_ZERO(&read_fds);
  FD_SET(fd, &read_fds);

  /* Initialize the timeout data structure. */
  timeout.tv_sec = tv_sec;
  timeout.tv_usec = tv_usec;
  return select(fd+1,&read_fds,NULL,NULL,&timeout) > 0;
}


void Usb::process_data(const char * data) {
    // 9/30 - greater than 8.8% wall time .23 ms duration
    static PerformanceData perf_data("Usb::process_data");
    MethodTracker t(perf_data);

  // this can be called with any amount of data, so we might have leftover data,
  // many lines, or no lines at all.  Handle all cases and send only complete lines
  // to listeners.
  for(const char * incoming = data; *incoming; ++incoming) {
    if(*incoming == '\n') {
      string s = ss.str();
      send_to_listeners(s);;
      // reset the stream
      ss.str("");
      ss.clear();
      ss.seekp(0); // for outputs: seek put ptr to start
      ss.seekg(0); // for inputs: seek get ptr to start
    } else {
      ss.put(*incoming);
    }
  }
}

void Usb::write_line(string text) {
  std::unique_lock<std::mutex> l(usb_mutex);
  string_pending_write += text + "\n";
}


void echo_off(string tty) {
  string command = (string) "stty -F " + tty + " -echo";
  if(system(command.c_str())!=0) throw "echo off failed";
}

// see https://www.pjrc.com/teensy/serial_listen.c
void make_raw(int fd) {
  struct termios settings;
  tcgetattr(fd, &settings);
  cfmakeraw(&settings);
  tcsetattr(fd, TCSANOW, &settings);
}

void Usb::monitor_incoming_data_thread() {
  log_info("begining usb monitor thread");
  try {
    monitor_incoming_data();
    log_info("exiting usb monitor thread normally");
  }
  catch(...) {
    log_error("usb monitoring thread exiting because of unknown exception");
  }

}

void Usb::monitor_incoming_data() {
  const int buf_size = 200;
  char buf[buf_size];
  const int poll_us = 1000;
  const int max_wait_us = 2E6; // two second

  //fstream usb;
  const int fd_error = -1;
  auto fd = fd_error;
  while(!quit) {
    // find and open usb
    for(string usb_path : glob(_device_path)) {
      echo_off(usb_path);
      fd = open(usb_path.c_str(), O_RDWR | O_NONBLOCK | O_SYNC | O_APPEND | O_NOCTTY);
      if(fd != fd_error) {
        make_raw(fd);
        if(_write_on_connect.size() > 0) {
          write_line(_write_on_connect);
        }
        break;
      }
    }

    // read until we hit an error a a quit
    int us_waited = 0;
    while(fd != fd_error && ! quit) {

      if(fd!=fd_error && !quit && string_pending_write.size() > 0) {
        std::unique_lock<std::mutex> l(usb_mutex); // lock while we use string_pending_write
        if(write(fd,string_pending_write.c_str(),string_pending_write.size()) != fd_error) {
          string_pending_write = "";
        } else {
          close(fd);
          fd = fd_error;
        }
      }

      bool did_work = false;
      ssize_t count = 0;

      if(fd != fd_error && wait_for_data(fd)) {
        count = read(fd, buf, buf_size-1); // read(2)
        if(count==fd_error) {
          count = 0;
          close(fd);
          fd = fd_error;
        }
        buf[count]=0;
      }
      if(count > 0) {
        process_data(buf);
      }
    }

    if(fd!=fd_error) {
      close(fd);
    }
    if(!quit){
      usleep(poll_us*10);
    }
  }
}


void Usb::run(std::string device_path){
  _device_path = device_path;

  run_thread = thread(&Usb::monitor_incoming_data_thread,this);
  pthread_setname_np(run_thread.native_handle(), "car_usb_run");
}

void Usb::flush()
{
  for(int i = 0; i < 1000; i++) {
    if(this->string_pending_write.size() == 0)
      break;
    this_thread::sleep_for(chrono::milliseconds(1));
  }
}

void Usb::stop(){
  flush();
  // kill processing thread
  quit = true;
  if(run_thread.joinable())
    run_thread.join();
}


void write_to_usb(Usb* usb, float seconds_between) {
  int i = 0;
  while(true) {
    usb->write_line("pse 1500,1500");
    usleep(seconds_between*1E6);
    i++;
    if(i%1000 == 0) {
      cout << "wrote " << i << " commands" << endl;
    }
  }
}

#include <thread>
void test_usb() {
  cout << "test usb" << endl;
  Usb usb;
  usb.write_on_connect("\ntd+\n");
  WorkQueue<StampedString> q;
  usb.add_line_listener(&q);
  cout << "about to run usb " << endl;
  usb.run("/dev/ttyACM1");
  StampedString s;
  int i = 0;

  auto t_start = high_resolution_clock::now();

  usb.write_line("td+");
  usb.write_line("td+");
  cout << "entering loop for usb" << endl;
  thread write_thread(write_to_usb,&usb,0.001);
  while(q.try_pop(s, 100)) {

    auto d = high_resolution_clock::now()-t_start;
    duration<double> secs = duration_cast<duration<double>>(d);
    if(i%100 == 0) {
      cout << "time: " << secs.count() << " received " << i << " lines" << endl;
    }
    i++;
  }
  usb.stop();
  cout << "timed out waiting for data" << endl;
}
