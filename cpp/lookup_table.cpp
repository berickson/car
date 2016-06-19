#include "lookup_table.h"
#include <vector>
#include <array>
#include <geometry.h>

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
  for(unsigned i = 0; i < last-1; i++) {
    if (v >= table[i+1][0]){
      return interpolate(v,table[i][0],table[i][1],table[i+1][0],table[i+1][1]);
    }
  }
  return NAN; // error if we didnt find v

}
