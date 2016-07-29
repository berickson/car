#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QTimer>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "opencv2/videoio.hpp"
#include "opencv2/calib3d.hpp"
#include "../tracker.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  void on_actionExit_triggered();

  void process_one_frame();

  void on_routesButton_clicked();

private:
  QTimer timer;
  Ui::MainWindow *ui;
  cv::VideoCapture cap;

  int frame_count = 0;
  cv::Mat frame;
  Tracker tracker;

};

#endif // MAIN_WINDOW_H
