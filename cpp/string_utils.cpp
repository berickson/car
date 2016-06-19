#include "string_utils.h"
#include <sstream>

using namespace std;
string join(vector<string> strings, string separator) {
  bool first = true;
  stringstream ss;
  for(string s:strings) {
    if(!first)
      ss << separator;
    ss << s;
    first=false;
  }
  return ss.str();
}

string format(double d) {
  stringstream ss;
  ss<<d;
  return ss.str();
}
