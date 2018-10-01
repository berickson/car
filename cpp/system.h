#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <list>
#include <vector>
#include <fstream>
#include <chrono>


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
string get_home_folder();

std::vector<std::string> glob(const string& pat);


// returns a formated time string for time point
std::string time_string(std::chrono::system_clock::time_point tp);

// returns a formatted time string for now
string time_string();
chrono::system_clock::time_point time_from_string(string s);

void test_system();


#endif // SYSTEM_H
