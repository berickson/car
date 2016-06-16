#include "string_utils.h"

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
