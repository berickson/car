#include "config.h"
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include "string_utils.h"
#include "logger.h"

using namespace std;

Config::Config() {

}

void Config::load_from_file(string path) {
  log_entry_exit w("config::load_from_file");
  fstream fs;
  fs.open(path,fstream::in);
  if(fs.fail()) {
    log_error((string) "could not open config file " + path );
    throw (string) "could not open config file " + path;
  }

  log_info("parsing configuration");

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
  log_trace((string) "loading " + s);

  if(_data.find(s) == _data.end()) {
    log_error((string)"config has no entry for" + s);
  }
  return std::stoi(_data[s]);
}

double Config::get_double(string s) {
  log_trace((string) "loading ");
  log_trace(s);

  if(_data.find(s) == _data.end()) {
    log_error((string)"config has no entry for" + s);
  }
  return std::stod(_data[s]);
}


void test_config() {
  Config config;
  config.load_from_file("/home/brian/car/python/car.ini");

  cout << "wheelbase_length_in_meters: " << config.get_double("wheelbase_length_in_meters") << endl;
}
