#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <sstream>
#include "trim.h"
#include "split.h"


using namespace std;

string join(vector<string> strings, string separator = ",");
string format(double d);

#endif // STRING_UTILS_H
