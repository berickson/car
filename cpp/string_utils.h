#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

using namespace std;

string join(vector<string> strings, string separator = ",");
string ftos(double d, int precision);
string format(double d);
string format(double d, int width, int precision);
vector<string> split(const string& s, const string & delim_string);
vector<string> split(const string& s, char delim_char=',', bool all = true);


// trim from start (in place)
void ltrim(std::string &s);

// trim from end (in place)
void rtrim(std::string &s);

// trim from both ends (in place)
void trim(std::string &s);

// trim from start (copying)
std::string ltrimmed(std::string s);

// trim from end (copying)
std::string rtrimmed(std::string s);

// trim from both ends (copying)
std::string trimmed(std::string s);

string to_fixed_width_string(int n, int width, char fill);

template <class T>
string vector_to_string(const vector<T> &v) {
  if (v.size() == 0) {
    return "";
  }
  string rv;
  rv += "[";
  rv += to_string(v[0]);
  for (int i = 1; i < v.size(); ++i) {
    rv += (string) ", " + to_string(v[i]);
  }
  rv += "]";
  return rv;
}


#endif // STRING_UTILS_H
