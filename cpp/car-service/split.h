#ifndef SPLIT_H
#define SPLIT_H
#include <vector>
#include <string>
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



#endif // SPLIT_H
