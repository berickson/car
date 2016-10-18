#include "system.h"
#include <unistd.h> // usleep
#include <list>
#include <string>
#include <iostream>
#include <algorithm>
#include <iomanip> // put_time

#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <dirent.h>

#include <ctime>
#include <chrono>
#include <sstream>


using namespace std;


System::System()
{

}

// based on http://stackoverflow.com/a/265978/383967,
// modified return only non-local ipv4
list<string> get_ip_addresses() {
    list<string> addresses;
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if(ifa->ifa_name == string("lo")) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            addresses.push_back(addressBuffer);
        }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
    return addresses;
}


void test_get_ip_addresses() {
    auto l = get_ip_addresses();
    for(auto s:l)
        cout << s << endl;
}

// returns one ip address from this machine or the text
// "not connected"
string get_first_ip_address() {
  auto addresses = get_ip_addresses();
  if(addresses.size()>0)
    return addresses.front();
  return "not connected";
}



// returns sorted list of child folders, empty list if folder doesn't exist
std::vector<std::string> child_folders (string folder) {
  vector<string> rv;
  DIR *dir = opendir(folder.c_str());
  if(!dir) {return rv;};
  struct dirent *entry = readdir(dir);

  while (entry != NULL) {
    if (entry->d_type == DT_DIR && entry->d_name[0] != '.'){
      string s = string(entry->d_name);
      rv.push_back(s);
    }
    entry = readdir(dir);
  }

  closedir(dir);
  std::sort(rv.begin(),rv.end());
  return rv;
}

bool file_exists(string path) {
  ifstream f(path.c_str());
  return f.good();

}

string path_join(string x, string y) {
  if(x[x.length()-1]=='/') {
    return x+y;
  } else {
    return x+"/"+y;
  }
}

void mkdir(string folder) {
  string cmd = "mkdir \""+folder+"\"";
  system(cmd.c_str());
  if(!file_exists(folder)) {
    throw (string) "could not create " + folder;
  }
}

string get_home_folder() {
    return (string) getenv("HOME");
}



#include "glob.h"
// http://stackoverflow.com/a/8615450/383967
std::vector<std::string> glob(const string& pat){
    using namespace std;
    glob_t glob_result;
    glob(pat.c_str(),GLOB_TILDE,NULL,&glob_result);
    vector<string> ret;
    for(unsigned int i=0;i<glob_result.gl_pathc;++i){
        ret.push_back(string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);
    return ret;
}



// Returns an ISO 8601 datetime in UTC to milliseconds resolution
string time_string(std::chrono::system_clock::time_point &tp)
{
  time_t tt = std::chrono::system_clock::to_time_t(tp);
  chrono::system_clock::duration d = tp.time_since_epoch();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(d);


  std::stringstream ss;
  ss << std::put_time(std::gmtime(&tt), "%Y-%m-%dT%X");
  ss << "." << setw(3) << setfill('0') << ms.count() % 1000;
  ss << "Z";
  return ss.str();
}

string time_string() {
  std::chrono::system_clock::time_point t = std::chrono::system_clock::now();

  return time_string(t);
}


void test_system() {
  test_get_ip_addresses();
  cout << "home folder: " << get_home_folder() << endl;
  for(int i = 0; i < 2000; i++) {
    cout << "time_string(): " << time_string() << endl;
    usleep(1000);
  }
}
