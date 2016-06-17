#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <list>
#include <vector>
#include <fstream>



using namespace std;

class System
{
public:
    System();
};

string path_join(string x, string y);

// returns true if file at path exists
bool file_exists(string path);

vector<string> child_folders(string folder);


list<string> get_ip_addresses();
string get_first_ip_address();
void mkdir(string folder);
void test_system();

#endif // SYSTEM_H
