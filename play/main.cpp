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

void copy_lines(const char * src_path, const char * dest_path) {
  ifstream src;
  ofstream dest;
  src.open(src_path);
  dest.open(dest_path);

  string line;  
  
  timespec t_wait;
  t_wait.tv_sec = 0;
  t_wait.tv_nsec = 1000000;
//  timespec t_rem;
  
  while(getline(src,line)) {
    dest << line << endl;
    nanosleep(&t_wait,NULL);
    //usleep(10);
  }
}


int main() {

  int count = 20000;  
  const char * src_path = "deleteme.txt";
  char const * dest_path = "deleteme2.txt";
  
  write_lines(src_path, count);
  copy_lines(src_path, dest_path);

  return 0;
}
