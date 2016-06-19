#ifndef CONFIG_H
#define CONFIG_H

#include <map>

using namespace std;
class Config {
public:
  Config();

  map<string,string> _data;

  void load_from_file(string path);

  int get_int(string s);
  double get_double(string s);
};

void test_config();

#endif // CONFIG_H
