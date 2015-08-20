#include <iostream>
#include <string>
#include <vector>
using namespace std;
const int token_count = 3;

struct stream_scrubber {
  string tokens[3];
  stream_scrubber() :
    tokens({"ln","cn","ln"}) {
  }

  bool matches[token_count] = {true,false,false};

  void add_token(string token){

    // set a match if current and previous both matched
    for(int i = token_count -1; i > 0; --i) {
      matches[i] = (matches[i-1] && tokens[i] == token);
    }
    // special case for the first token since there is no prior match
    if(tokens[0] == token) {
      matches[0] = true;
    } else {
      matches[0] = false;
    }
  }

  bool found_match() {
    return matches[token_count-1];
  }

  void dump() {
    for(int i=0;i<token_count; i++){
      cout << tokens[i] << ":" << matches[i] << endl;
    }
  }
};


int main() {
  cout << "hello, world!" << endl;

  vector<string> v = {"ln","cn","rn","cn","ln","cn","ln"};
  stream_scrubber scrubber;

  scrubber.dump();
  for(string t: v){
    scrubber.add_token(t);
    cout << "added "<< t << endl;
    scrubber.dump();
    if(scrubber.found_match()) {
      cout << "found a match!" << endl;
    }
  }

  return 0;
}
