#include "usb.h"
#include <fcntl.h> // for open,close
#include <unistd.h> // for file operations, usleep
#include <iostream>
#include <string>
#include <thread>
#include "glob_util.h"
#include "ends_with.h"
#include "split.h"

using namespace std;

void Usb::add_line_listener(WorkQueue<string>*listener){
  line_listeners.push_back(listener);
}

void Usb::remove_line_listener(WorkQueue<string>*listener){
  line_listeners.remove(listener);
}

void Usb::send_to_listeners(string s) {
  for(WorkQueue<string>* listener : line_listeners) {
    listener->push(s.c_str());
  }
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
  if(has_newline) {
    send_to_listeners(lines[last_index]);
  } else {
    leftover_data = lines[last_index];
  }
}

void Usb::monitor_incoming_data() {
  const int error = -1; // error constant for file operations
  const int buf_size = 2000;
  char buf[buf_size];
  const int poll_us = 1000;    // one millisecond
  const int max_wait_us = 1E6; // one second

  while(!quit) {
    int fd = error;
    // find and open usb
    for(string usb_path : glob("/dev/ttyACM*")) {
      fd = open(usb_path.c_str(), O_RDONLY | O_NONBLOCK);
      if(fd != error) {
        //cout << "connected to port" << usb_path << endl;
        break;
      }
    }

    //if(fd == error)
    //  cout << "couldn't find a port" << endl;

    // read until we hit an error a a quit
    int us_waited = 0;
    while(fd != error && ! quit) {
      bool did_work = false;
      auto count = read(fd, buf, buf_size-1); // read(2)

      if(count == error ) {
        break;
      }
      buf[count]=0;
      if(count > 0) {
        did_work = true;
        us_waited = 0;
        process_data(buf);
      }
      if(!did_work) {
        if(us_waited > max_wait_us) // one second
          break;
        usleep(poll_us); // wait 1 ms for more data
        us_waited += poll_us;
      }
    }

    if(fd != error) {
      //cout << "closing usb" << endl;
      close(fd);
    }
    if(!quit){
      usleep(poll_us*10);
    }
  }
}


void Usb::run(){
  run_thread = thread(&Usb::monitor_incoming_data,this);
}

void Usb::stop(){
  quit = true;
  if(run_thread.joinable())
    run_thread.join();
}


void test_usb() {
  Usb usb;
  WorkQueue<string> q;
  usb.add_line_listener(&q);
  usb.run();
  string s;
  while(q.try_pop(s, 15000)) {
    cout << "got item " << s << endl;
  }
  cout << "timed out once" << endl;
  if(q.try_pop(s, 15000)) {
    cout << s << endl;
  }
  usb.stop();
  cout << "timed out waiting for data" << endl;
}
