#include "string_utils.h"
#include <sstream>

#include <iomanip>

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

string format(double d, int width, int precision) {
  stringstream ss;
  ss<< std::fixed << std::setw( width ) << std::setprecision( precision )
   <<  d;
  return ss.str();
}
