#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <list>


using namespace std;

class System
{
public:
    System();
};


list<string> get_ip_addresses();
string get_first_ip_address();

void test_system();

#endif // SYSTEM_H
