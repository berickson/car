#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h> // for read, usleep

using namespace std;

int main(int, char **) {
  int fd = open("//dev//ttyACM0",O_RDWR);
  cout << "fd:"<< fd << endl;
  char buf[2000];
  bool last_bytes_read = 0;
  while(1) {
    for(int i = 0; i < 100000; ) {
      ssize_t bytes_read = read(fd, buf, sizeof(buf)-1);
      buf[bytes_read]=0;
      if(bytes_read == -1) {
        cout << endl << "error reading " << endl;
      }
      
      // expecting every 10 ms, so wait 8 ms then poll fast
      if(bytes_read==0) {
        if(last_bytes_read > 0)
          usleep(8000);
        else
          usleep(1000);
      }
      last_bytes_read = bytes_read;
      if(bytes_read>0) {
         cout << buf;
         i+= bytes_read;
      }
    }
    cerr << ".";
  }  

}
