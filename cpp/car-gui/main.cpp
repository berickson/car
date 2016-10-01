#include "main_window.h"
#include "route_window.h"
#include "stereo_window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  //RouteWindow w;
  //MainWindow w;
  StereoWindow w;
  w.show();

  return a.exec();
}
