  
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <iostream>
#include <sys/poll.h>
#include <vector>


#include <iostream>
#include <fstream>
#include <unistd.h> // for usleep
#include <time.h>

using namespace std;

void write_lines(const char * dest_path, int count) {
  ofstream f;
  f.open(dest_path);
  for(int i=0;i<count;i++)
    f << i << " hello, there." << endl;
}


void sleep() {
  timespec t_wait;
  t_wait.tv_sec = 0;
  t_wait.tv_nsec = 1000000;
  nanosleep(&t_wait,NULL);
}

int copy_lines(const char * src_path, const char * dest_path) {
  int lines_copied = 0;
  ifstream src;
  ofstream dest;

  string line;  
  
//  timespec t_rem;
  
  src.open(src_path);
  dest.open(dest_path,std::ofstream::app);
  char buffer[1000];
  std::string s;
  while(lines_copied < 300000) {
    buffer[0] = 0;
    src.getline(buffer,999);
    if(src.eofbit)
      s += string(buffer);
      sleep();
      cout << "+" << s << endl;
      s.clear();
      continue;
    if(src.failbit)
      sleep();
      continue;
    cout << lines_copied << "." << s << buffer << endl;
    s.clear();
    lines_copied++;
    
  }
  src.close();
  dest.close();
  return lines_copied;
}


int readAll(int fd)
{
	unsigned char buf[256];
	int res;
	int count = 0;
	while(1)
	{
		res = read( fd, &buf, 100 );
		if (res == 0) return count;
		if (res < 0) return -1;
		count += res;
//		cout << ".";
		cout.flush();
	}
}


int open(const char *device)
{
	tcflag_t bitrate = B115200;
	int fd = ::open( device, O_RDWR | O_NOCTTY );
	if ( fd < 0 )
    {
        fprintf( stderr, "Failed to open serial device '%s' (errno: %s)\n", device, strerror(errno) );
        return -1;
    }
    if (::ioctl(fd, TIOCEXCL))
    {
        fprintf( stderr, "Failed to lock serial device '%s' (errno: %s)\n", device, strerror(errno) );
        return -1;
    }



    // Check if device is a terminal device
    if ( !isatty( fd ) )
    {
        fprintf( stderr, "Device '%s' is not a terminal device (errno: %s)!\n", device, strerror(errno) );
        ::close( fd );
        return -1;
    }

    struct termios settings;
    // Set input flags
    settings.c_iflag =  IGNBRK          // Ignore BREAKS on Input
                     |  IGNPAR;         // No Parity
                                        // ICRNL: map CR to NL (otherwise a CR input on the other computer will not terminate input)

    // Set output flags
    settings.c_oflag = 0;				// Raw output

    // Set controlflags
    settings.c_cflag = bitrate
                     | CS8              // 8 bits per byte
                     | CSTOPB			// Stop bit
                     | CREAD            // characters may be read
                     | CLOCAL;          // ignore modem state, local connection

    // Set local flags
    settings.c_lflag = 0;				// Other option: ICANON = enable canonical input

    // non-canonical mode: set device to non blocking mode:
	// Set timer
    settings.c_cc[VTIME] = 0;			// 0 means timer is not used

    // Set minimum bytes to read
    settings.c_cc[VMIN]  = 0;			// 1 means wait until at least 1 character is received; 0 means don't wait

    // Now clean the modem line and activate the settings for the port
    tcflush( fd, TCIFLUSH );
    tcsetattr( fd, TCSANOW, &settings );
	return fd;
}

#include "sched.h"

void example2() {
  int fd = ::open("/dev/ttyACM0");
  
  timespec t_wait;
  t_wait.tv_sec = 0;
  t_wait.tv_nsec = 1000000;
 
  
  while(1) {
    readAll(fd);
    //sched_yield() ;//cpu_relax();
  }
  
}

void transcribe_usb() {
    copy_lines("/dev/ttyACM0", "usb_out.txt");
}


int main() {
//  example2();
  transcribe_usb();
  return 0;
}
