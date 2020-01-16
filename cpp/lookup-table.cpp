#include "lookup-table.h"
#include <vector>
#include <array>
#include "geometry.h"
#include <iostream>

LookupTable::LookupTable(vector<array<double, 2> > table)
  : table(table)
{

}

double LookupTable::lookup(double v) const {
  unsigned last = table.size()-1;
  if (v <= table[0][0]){
    return table[0][1];

  }
  if (v >= table[last][0]) {
    return table[last][1];
  }
  for(unsigned i = 0; i < last; i++) {
    if (v >= table[i][0] && v < table[i+1][0]){
      return interpolate(v,table[i][0],table[i][1],table[i+1][0],table[i+1][1]);
    }
  }
  return NAN; // error if we didnt find v

}

void test_lookup_table()
{
  LookupTable t(
  {
    {-2., 1200},
    {-1., 1250},
    {0.0,  1500},
    {0.1, 1645},
    {0.34, 1659},
    {0.85, 1679},
    {1.2, 1699},
    {1.71, 1719},
    {1.88, 1739},
    {2.22, 1759},
    {2.6, 1779},
    {3.0, 1799},
    {14.0, 2000}
  });

  vector<double> items = {-3,-2,0,2.1,4,13.9,16};
  for (auto item:items)
    cout << "lookup " << item << " returned " << t.lookup(item) << endl;
}
