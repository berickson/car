#include "usb.h"
//#include <fcntl.h> // for open,close
#include <unistd.h> // for file operations, usleep
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include "glob_util.h"
#include "ends_with.h"
#include "split.h"

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
  string stamped = "0,"+s;
  for(WorkQueue<string>* listener : line_listeners) {
    listener->push(stamped.c_str());
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

void Usb::monitor_incoming_data() {
  const int buf_size = 20000;
  char buf[buf_size];
  const int poll_us = 3000;
  const int max_wait_us = 2E6; // two second

  fstream usb;
  while(!quit) {
    // find and open usb
    for(string usb_path : glob("/dev/ttyACM*")) {
      usb.open(usb_path,std::fstream::in|std::fstream::out );
      if(usb.is_open()) {
				write_line(_write_on_connect);        
				break;
      }
    }

    // read until we hit an error a a quit
    int us_waited = 0;
    while(usb.is_open() && ! quit) {

      if(usb.is_open() && !quit && pending_write.size() > 0) {
        usb<<pending_write<<flush;
        pending_write = "";
      }
      bool did_work = false;
      int count = 0;
      if(usb.good()) {
        count = usb.readsome(buf,sizeof(buf)-1);
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
      }
    }

    if(usb.is_open()) {
      usb.close();
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
