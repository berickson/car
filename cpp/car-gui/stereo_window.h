#ifndef STEREO_WINDOW_H
#define STEREO_WINDOW_H

#include <QDialog>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "opencv2/videoio.hpp"
#include "opencv2/calib3d.hpp"
#include <QLabel>
#include <vector>
#include "../camera.h"

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
  void on_show_features_checkbox_toggled(bool);

  void on_match_features_checkbox_toggled(bool checked);

public:
  void show_frame(int number);
  int get_frame_number();

  Ui::StereoWIndow *ui;

  struct CameraUnit {
    Camera camera;
    std::string name;
    cv::VideoCapture cap;
    cv::Mat frame;
    cv::Mat gray;
    QLabel * bound_label;
    StereoWindow * parent;
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;


    void grab_frame(int frame_number);
    void detect_features(cv::Rect2d valid_Rect);
    void undistort_frame();
    void show();
    void show_match(int feature_index, cv::Scalar color);
  } left_camera, right_camera;


  std::vector<CameraUnit*> cameras {&left_camera, &right_camera};
};

#endif // STEREO_WINDOW_H
