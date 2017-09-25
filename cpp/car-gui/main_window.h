#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <vector>

#include <QMainWindow>
#include <QDialog>
#include <QTimer>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "opencv2/videoio.hpp"
#include "opencv2/calib3d.hpp"
#include "../tracker.h"

#include "../frame_grabber.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QDialog
{
  Q_OBJECT

public:

  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  void set_camera();

private slots:
  void on_actionExit_triggered();

  void process_one_frame();
  void fps_changed(int fps);

  void on_routesButton_clicked();

  void on_webcamButton_clicked();

  void on_brightness_slider_valueChanged(int value);

  void on_contrast_slider_valueChanged(int value);

  void on_hue_slider_valueChanged(int value);

  void on_saturation_slider_valueChanged(int value);

  void on_resolutions_combo_box_currentIndexChanged(int index);

  void on_video_device_currentTextChanged(const QString &arg1);

  void on_take_picture_button_clicked();

private:

  std::vector<QSize> supported_resolutions = {{320,240},{640,480},{800,600},{1024,768},{1280,720},{1920,1080}};

  QTimer timer;
  Ui::MainWindow *ui;
  cv::VideoCapture cap;
  FrameGrabber frame_grabber;

  int frame_count = 0;
  cv::Mat original_frame;
  cv::Mat frame;
  cv::Mat view;
  Tracker tracker;

};

#endif // MAIN_WINDOW_H

