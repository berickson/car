#include "usb.h"
#include <sys/stat.h>
#include <fcntl.h> // for open,close
#include <unistd.h> // for file operations, usleep
#include <iostream>
#include <termios.h> // for make_raw
#include <string>
#include <thread>
#include "ends_with.h"
#include "string_utils.h"
#include "system.h"
#include "logger.h"
#include "diagnostics.h"

using namespace std;
using namespace std::chrono;


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

void Usb::send_to_listeners(const string & s) {
  // 1/2/20 line_callback ms average_duration: 0.009609 ms% wall: 1.808009
  // 1/1/20 (coonst string &) average_duration: 0.018191 ms% wall: 3.248648
  // 1/1/20 (by value) average_duration: 0.019101 ms% wall: 3.366067
  // 9/30 - about 8.0%  clock wall time
  //      - time_string to int - about 2.6 % wall time
  //      - time StampedString 1.373905 % wall time
  // static PerformanceData perf_data("Usb::send_to_listeners");
  // MethodTracker t(perf_data);
  if(line_callback != nullptr) {
    line_callback(s.c_str());
    return;
  }
    

  StampedString payload(s, system_clock::now());

  for(auto listener : line_listeners) {
      // 9/30 - over 1.0%  clock wall time
      // removed time_string, 0.94%
      //static PerformanceData perf_data("Usb::send_to_listeners.push");
      //MethodTracker t(perf_data);
      listener->push(payload);
  }
}

// returns 0 if no data, > 0 if data, < 0 if error
int wait_for_data(int fd, __time_t tv_sec = 1, __suseconds_t tv_usec = 0) {
  struct timeval timeout;
  /* Initialize the file descriptor set. */
  fd_set read_fds;
  FD_ZERO(&read_fds);
  FD_SET(fd, &read_fds);

  /* Initialize the timeout data structure. */
  timeout.tv_sec = tv_sec;
  timeout.tv_usec = tv_usec;
  return select(fd+1,&read_fds,NULL,NULL,&timeout);
}


void Usb::process_data(const char * data) {
    // 9/30 - greater than 8.8% wall time .23 ms duration
    // removed time_string - 2.86% wall time
    // static PerformanceData perf_data("Usb::process_data");
    // MethodTracker t(perf_data);

  // send only complete lines to listeners
  for(const char * incoming = data; *incoming; ++incoming) {
    if(*incoming == '\n') {
      read_buffer.push_back(0);
      send_to_listeners(read_buffer.data());
      read_buffer.clear();
    } else {
      read_buffer.push_back(*incoming);
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
  log_info("begining usb monitor thread for " + _device_path);
  try {
    monitor_incoming_data();
    log_info("exiting usb monitor thread normally");
  }
  catch(string s) {
    log_error("usb monitoring thread exiting because of exception "  + _device_path);
    log_error(s);
  }
  catch(std::exception & e) {
    log_error("usb monitoring thread exiting because of standard exception"  + _device_path);
    log_error(e.what());
  }
  catch(...) {
    log_error("usb monitoring thread exiting because of unknown exception " + _device_path);
  }

}

void Usb::monitor_incoming_data() {
  const int buf_size = 200;
  char buf[buf_size];
  const int poll_us = 1E6; // one second
  const int max_wait_us = 2E6; // two second

  //fstream usb;
  const int fd_error = -1;
  auto fd = fd_error;
  while(!quit) {
    string usb_path = "not connected";
    // find and open usb
    for(string try_usb_path : glob(_device_path)) {
      echo_off(try_usb_path);
      fd = open(try_usb_path.c_str(), O_RDWR | O_NONBLOCK | O_SYNC | O_APPEND | O_NOCTTY);
      if(fd != fd_error) {
        make_raw(fd);
        usb_path = try_usb_path;
        log_info("connected to USB at "+usb_path);
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
        if(write(fd,string_pending_write.c_str(),string_pending_write.size()) >0) {
          string_pending_write = "";
        } else {
          log_warning("couldn't write to " + usb_path + ". Closing.");
          close(fd);
          fd = fd_error;
        }
      }

      bool did_work = false;
      ssize_t count = 0;

      if(fd != fd_error) {
        int wait_result = wait_for_data(fd);
        if(wait_result < 0) {
            log_warning("error waiting for " + usb_path + ". Closing.");
            close(fd);
            fd = fd_error;
        }
        if (wait_result > 0 ) {
          count = read(fd, buf, buf_size-1); // read(2)
          if(count<=0) {
            count = 0;
            log_warning("couldn't read from " + usb_path + "count returned "+ to_string(count) +". Closing");
            close(fd);
            fd = fd_error;
          } else {
            buf[count]=0;
          }
        }
      }
      if(count > 0) {
        process_data(buf);
      }
    }

    if(fd!=fd_error) {
      close(fd);
    }
    if(!quit){
      usleep(poll_us);
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

void Usb::set_line_callback(std::function <void(const char *)> line_callback) {
  this->line_callback = line_callback;
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


