#include "Statistics.h"

#include <iostream>


void print_stats(Statistics & s) {
  cout << "slope : " << s.slope() << endl; 
  cout << "stdx : " << s.stdx() << endl; 
  cout << "stdy : " << s.stdy() << endl; 
  cout << "correlation : " << s.correlation() << endl; 
  cout << "predicty(0) : " << s.predicty(0) << endl; 
  
}

TEST(Statistics,StatisticsTest) {
   cout << "this is the statistics test" << endl;
   Statistics s;
   
   cout << "(3,2),(4,3)" << endl;
   s.add(3,2);
   s.add(4,3);
   print_stats(s);

   cout << "(pow(2.,33) ,0),pow(2.,33)+1,1" << endl;
   
   s.reset();
   double startx = pow(2.,10);
   s.add(startx ,2);
   s.add(startx+1 , 3);
   print_stats(s);
   cout << "predicty("<<startx<<") : " << s.predicty(startx) << endl; 
   
   cout << "below should be int 26.7808 slope 0.6438" << endl;
   s.reset();
   s.add(95,85);
   s.add(85,95);
   s.add(80,70);
   s.add(70,65);
   s.add(60,70);
   print_stats(s);

   // http://www.algebralab.org/Word/Word.aspx?file=Algebra_LinearRegression.xml
   cout << "below should be slope 0.556 int 17.778" << endl;
   s.reset();
   s.add(32,0);
   s.add(68,20);
   s.add(86,30);
   s.add(122,50);
   s.add(158,70);
   s.add(194,90);
   s.add(212,100);
   
   print_stats(s);
   
   
}
