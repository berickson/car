#include "usb.h"
#include <sys/stat.h>
#include <fcntl.h> // for open,close
#include <unistd.h> // for file operations, usleep
#include <iostream>
#include <termios.h> // for make_raw
//#include <fstream>
#include <string>
#include <thread>
#include "ends_with.h"
#include "split.h"
#include "system.h"
#include "logger.h"

using namespace std;
using namespace std::chrono;

Usb::~Usb() {
  stop();
}

void Usb::write_on_connect(string s) {
  _write_on_connect = s;
}

void Usb::add_line_listener(WorkQueue<string>*listener){
  line_listeners.push_back(listener);
}

void Usb::remove_line_listener(WorkQueue<string>*listener){
  line_listeners.remove(listener);
}

void Usb::send_to_listeners(string s) {
  string stamped = time_string()+","+s;
  for(WorkQueue<string>* listener : line_listeners) {
    listener->push(stamped);
  }
}

bool data_available(int fd) {
  struct timeval timeout;
  /* Initialize the file descriptor set. */
  fd_set read_fds, write_fds, except_fds;
  FD_ZERO(&read_fds);
  FD_ZERO(&write_fds);
  FD_ZERO(&except_fds);
  FD_SET(fd, &read_fds);

  /* Initialize the timeout data structure. */
  timeout.tv_sec = 0;
  timeout.tv_usec = 15000;
  return select(fd+1,&read_fds,NULL,NULL,&timeout) > 0;
}


void Usb::process_data(string data) {
  // this can be called with any amount of data, so we might have leftover data,
  // many lines, or no lines at all.  Handle all cases and send only complete lines
  // to listeners.
  bool has_newline = ends_with(data, "\n");
  auto lines = split(data, '\n');

  // add leftover data from last time
  lines[0] = leftover_data + lines[0];
  leftover_data = "";

  // each array entry except the last is a complete line
  // so send them
  int last_index = lines.size() - 1;
  for(int i=0; i<last_index; i++) {
    send_to_listeners(lines[i]);
  }

  // send last line if it is complete, otherwise save it for later
  if(has_newline && lines[last_index].size() >1){
    send_to_listeners(lines[last_index]);
  } else {
    leftover_data = lines[last_index];
  }
}

void Usb::write_line(string text) {
  std::unique_lock<std::mutex> l(usb_mutex);
  pending_write += text + "\n";
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
    for(string usb_path : glob("/dev/ttyACM*")) {
      echo_off(usb_path);
      fd = open(usb_path.c_str(), O_RDWR | O_NONBLOCK | O_SYNC | O_APPEND | O_NOCTTY);
      if(fd != fd_error) {
        make_raw(fd);
				write_line(_write_on_connect);        
				break;
      }
    }

    // read until we hit an error a a quit
    int us_waited = 0;
    while(fd != fd_error && ! quit) {

      if(fd!=fd_error && !quit && pending_write.size() > 0) {
        if(write(fd,pending_write.c_str(),pending_write.size()) != fd_error) {
          pending_write = "";
        } else {
          close(fd);
          fd = fd_error;
        }
      }

      bool did_work = false;
      ssize_t count = 0;

      if(fd != fd_error && data_available(fd)) {
        count = read(fd, buf, buf_size-1); // read(2)
        if(count==fd_error) {
          count = 0;
          close(fd);
          fd = fd_error;
        }
        buf[count]=0;
      }
      if(count > 0) {
        did_work = true;
        us_waited = 0;
        process_data(buf);
      }
      if(!did_work) {
        us_waited += poll_us;
        if(us_waited > max_wait_us) // one second
          break;
        usleep(poll_us);
        us_waited += poll_us;
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


void Usb::run(){
  run_thread = thread(&Usb::monitor_incoming_data_thread,this);
}

void Usb::stop(){
  quit = true;
  if(run_thread.joinable())
    run_thread.join();
}


void test_usb() {
  cout << "test usb" << endl;
  Usb usb;
  usb.write_on_connect("\ntd+\n");
  WorkQueue<string> q;
  usb.add_line_listener(&q);
  cout << "about to run usb " << endl;
  usb.run();
  string s;
  int i = 0;

  auto t_start = high_resolution_clock::now();

  usb.write_line("td+");
  usb.write_line("td+");
  cout << "entering loop for usb" << endl;
  while(q.try_pop(s, 15000)) {
    auto d = high_resolution_clock::now()-t_start;
    duration<double> secs = duration_cast<duration<double>>(d);
    cout << secs.count() << "got item " << s << endl;
    cout << flush;
    i++;
  }
  usb.stop();
  cout << "timed out waiting for data" << endl;
}
