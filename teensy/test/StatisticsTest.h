#include "Statistics.h"

#include <iostream>


void print_stats(Statistics & s) {
  cout << "slope : " << s.slope() << endl; 
  cout << "stdx : " << s.stdx() << endl; 
  cout << "stdy : " << s.stdy() << endl; 
  cout << "correlation : " << s.correlation() << endl; 
  cout << "predicty(0) : " << s.predicty(0) << endl; 
  
}

TEST(Statistics,large_x_values) {
   Statistics s1,s2;
   
   cout << "(3,2),(4,3)" << endl;
   s1.add(0,2);
   s1.add(1,3);
   print_stats(s1);
   
   double startx = pow(2.,10);
   s2.add(startx ,2);
   s2.add(startx+1 , 3);
   print_stats(s2);
   cout << "predicty("<<startx<<") : " << s2.predicty(startx) << endl; 
   
   ASSERT_DOUBLE_EQ(s1.slope(),s2.slope());
   ASSERT_DOUBLE_EQ(s1.predicty(0), s2.predicty(startx));
}
   
TEST(Statistics,linear_regression) {
   cout << "below should be int 26.7808 slope 0.6438" << endl;
   Statistics s;
   s.add(95,85);
   s.add(85,95);
   s.add(80,70);
   s.add(70,65);
   s.add(60,70);
   print_stats(s);
   ASSERT_NEAR(26.7808, s.predicty(0),0.0001);
   ASSERT_NEAR(0.6438, s.slope(), 0.0001);
}

TEST(Statistics,algebralab_example) {
   // http://www.algebralab.org/Word/Word.aspx?file=Algebra_LinearRegression.xml
   cout << "below should be slope 0.556 intercept -17.778" << endl;
   Statistics s;
   s.add(32,0);
   s.add(68,20);
   s.add(86,30);
   s.add(122,50);
   s.add(158,70);
   s.add(194,90);
   s.add(212,100);
   
   ASSERT_NEAR(0.556, s.slope(),0.001);
   EXPECT_NEAR(-17.778, s.predicty(0), 0.001);
   
   print_stats(s);  
}

TEST(Statistics, unity_slope) {
  Statistics s;
  s.add(3,3);
  s.add(5,5);
  EXPECT_EQ(s.slope(),1) << "two points with slope of one";
  cout << "the slope was " << s.slope() << endl;
}

