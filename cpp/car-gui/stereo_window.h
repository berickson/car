#ifndef STEREO_WINDOW_H
#define STEREO_WINDOW_H

#include <QDialog>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "opencv2/videoio.hpp"
#include "opencv2/calib3d.hpp"

namespace Ui {
class StereoWIndow;
}

class StereoWindow : public QDialog
{
  Q_OBJECT

public:
  explicit StereoWindow(QWidget *parent = 0);
  ~StereoWindow();

private slots:
  void on_frame_slider_valueChanged(int value);

private:
  void show_frame(int number);
  cv::VideoCapture cap_l, cap_r;
  cv::Mat left_frame,right_frame;
  Ui::StereoWIndow *ui;
};

#endif // STEREO_WINDOW_H
