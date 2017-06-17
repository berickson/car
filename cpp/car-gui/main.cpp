#include "main_window.h"
#include "route_window.h"
#include "stereo_window.h"
#include "picker_window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  PickerWindow w;
  QRect picker_position;
  while(true) {
      w.show();
      if(picker_position.left() > 0) {
          w.setGeometry(picker_position);
      }
      w.exec();
      picker_position = w.geometry();
      if(w.result() != QDialog::Accepted) {
          break;
      }
      RouteWindow x;
      x.show();
      x.exec();
  }
  //MainWindow w;
  //StereoWindow w;
  //w.show();

  return 0;
}
