#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <thread>
#include <unistd.h> // for usleep, etc

#include "ackerman.h"

using namespace std;


vector<string> split(string str, char delimiter=',') {
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;

  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }

  return internal;
}



class Printer {
public:
    string text;
    void run() {
        cout << text << endl;
    }
};

int main(int, char *[])
{
    arc_to_relative_location_tests();

    string s = "TD,939.27,-32,Heading String,5";
    cout << "splitting string " << s << endl;
    for(auto i: split(s)){
        cout << i << endl;
    }


    Printer p;
    p.text = "calling a member from a thread";
    thread t1(&Printer::run, p);
    t1.join();
    return 0;
}
