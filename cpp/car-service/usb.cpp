#include "usb.h"
#include <fcntl.h> // for open,close
#include <unistd.h> // for file operations, usleep
#include <iostream>
#include <string>

using namespace std;

void Usb::run(){
  // open
  const int error = -1;
  string usb_path = "/dev/ttyACM0";
  int fd = open(usb_path.c_str(), O_RDONLY | O_NONBLOCK);
  if(fd == error) {
    cout << "could not open "<< usb_path << endl;
    return;
  }
  cout << "listening on " << usb_path << endl;

  try {
    const int buf_size = 2000;
    char buf[buf_size];
    // read
    while(1) {
      auto count = read(fd, buf, buf_size-1); // read(2)
      if(count == error) {
        cout << endl << "error reading " << usb_path << endl;
        break;
      }
      buf[count]=0;
      if(count > 0){
        cout << buf;
      }
      usleep(1000);
    }
  } catch (...) {}
  close(fd);
  cout << "closed file" << endl;
}
