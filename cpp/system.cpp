#include "system.h"
#include <list>
#include <string>
#include <iostream>


#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

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

void test_system() {
    test_get_ip_addresses();
}
