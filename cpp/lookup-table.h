#ifndef LOOKUPTABLE_H
#define LOOKUPTABLE_H
#include <vector>
#include <array>

using namespace std;
class LookupTable
{
public:
  LookupTable(vector<array<double, 2> > table);
  vector<array<double,2>> table;
  double lookup(double v) const;
};

void test_lookup_table();

#endif // LOOKUPTABLE_H
