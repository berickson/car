#ifndef SPLIT_H
#define SPLIT_H
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

inline vector<string> split(const string& s, char delim_char=',') {
  string delim = string(1, delim_char);
  vector<string> items;
  string::const_iterator substart = s.begin(), subend;
  while (true) {
    subend = search(substart, s.end(), delim.begin(), delim.end());
    string temp(substart, subend);
    items.push_back(temp);
    if (subend == s.end()) {
      break;
    }
    substart = subend + delim.size();
  }
  return items;
}


inline void test_split() {
  string s = "a,b,c,,";
  auto a = split(s,',');
  cout << "splitting \"" << s << "\" produced: " <<  endl;
  for(auto t:a) {
    cout << "\"" << t << "\"" << endl;
  }
}

#endif // SPLIT_H
