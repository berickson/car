#ifndef SPLIT_H
#define SPLIT_H
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

inline vector<string> split(string str, char delimiter=',') {
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;

  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }

  return internal;
}


inline void test_split() {
  string s = "a,b,c";
  auto a = split(s,',');
  cout << "splitting \"" << s << "\" produced: " <<  endl;
  for(auto t:a) {
    cout << "\"" << t << "\"" << endl;
  }
}

#endif // SPLIT_H
