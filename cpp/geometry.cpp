#include "geometry.h"
#include "iostream"

void test_geometry() {
  Angle a;
  cout << "setting a to 30 degrees" << endl;
  a.set_degrees(30);
  cout << "a: " << a.to_string() << endl;
  cout << "a.degrees(): " << a.degrees() << endl;
  cout << "a.radians(): " << a.radians() << endl;
  cout << " a * 2" << (a * 2).to_string();
  a+=Angle::degrees(360);
  cout << "adding 360 degrees" << endl;
  cout << "a.degrees(): " << a.degrees() << endl;
  a.standardize();
  cout << "a.standardize() " << endl;
  cout << "a.degrees(): " << a.degrees() << endl;
  cout << "a /= 2" <<endl;
  a/=2.;
  cout << "a.degrees(): " << a.degrees() << endl;
  cout << "a.to_string(): " << a.to_string() << endl;

  a = Angle::degrees(10);
  Angle b = Angle::degrees(20);
  cout << "a + b " << (a+b).degrees() << endl;


}
