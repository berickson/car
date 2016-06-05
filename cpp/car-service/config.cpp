#include "config.h"
#include <map>
#include <iostream>
#include <fstream>
#include "split.h"
#include <string>

#include <trim.h>

using namespace std;

Config::Config() {

}

void Config::load_from_file(string path) {
  fstream fs;
  fs.open(path,fstream::in);

  for (string line; getline(fs, line); ) {
    auto kv = split(line,'=');
    if(kv.size()==2) {
      string k = trimmed(kv[0]);
      string v = trimmed(kv[1]);
      if(k.length()==0) continue;
      if(v.length()==0) continue;
      if(k[0]=='#') continue;      // skip comments

      _data[k] = v;
    }
  }
}

int Config::get_int(string s) {
  return std::stoi(_data[s]);
}

double Config::get_double(string s) {
  return std::stod(_data[s]);
}


void test_config() {
  Config config;
  config.load_from_file("/home/brian/car/python/car.ini");

  cout << "wheelbase_length_in_meters: " << config.get_double("wheelbase_length_in_meters") << endl;
}
