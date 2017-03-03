#include "stereo_window.h"
#include "ui_stereo_window.h"
#include <qcheckbox.h>
#include <vector>


void show_image(cv::Mat & mat, QLabel & label ) {
  cv::cvtColor(mat, mat, CV_RGB2BGR);
  QImage imdisplay ((uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
  label.setFixedSize(imdisplay.size());

  QPixmap pixmap = QPixmap::fromImage(imdisplay);
  label.setPixmap(QPixmap::fromImage(imdisplay));
}

StereoWindow::StereoWindow(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::StereoWIndow)
{
  ui->setupUi(this);
  left_camera.cap.open("/home/brian/Desktop/tracks/desk/routes/X/runs/2/video_left.avi");
  right_camera.cap.open("/home/brian/Desktop//tracks/desk/routes/X/runs/2/video_right.avi");
  left_camera.bound_label = ui->left_image;
  left_camera.parent = this;
  right_camera.bound_label = ui->right_image;
  right_camera.parent = this;

  int frame_count = (int) left_camera.cap.get(cv::CAP_PROP_FRAME_COUNT);
  ui->frame_count->setText( QString::number(frame_count));
  ui->frame_slider->setMaximum(frame_count);
  ui->frame_slider->setValue(50);

}


StereoWindow::~StereoWindow()
{
  delete ui;
}

void StereoWindow::on_frame_slider_valueChanged(int value)
{
  show_frame(value);
}

void StereoWindow::show_frame(int number)
{
  ui->frame_slider->setValue(number);

  for(CameraUnit * camera : cameras) {
    camera->grab_frame(number);
  }

  if(ui->match_features_checkbox->isChecked()) {
    cv::BFMatcher matcher;
    std::vector<cv::DMatch> matches;
    matcher.match(left_camera.descriptors, right_camera.descriptors, matches);


    // sort the mathes by distance
    sort(matches.begin(), matches.end(),
        [](const cv::DMatch & a, const cv::DMatch& b) -> bool
    {
        return a.distance < b.distance;
    });


    for(unsigned int i = 0; i < matches.size() / 5; i++ ) {
      cv::DMatch match = matches.at(i);
      left_camera.show_match(match.queryIdx);
      right_camera.show_match(match.trainIdx);
    }
  }

  left_camera.show();
  right_camera.show();


}

int StereoWindow::get_frame_number()
{
  return ui->frame_slider->value();
}

void StereoWindow::on_show_features_checkbox_toggled(bool )
{
    show_frame(get_frame_number());
}

void StereoWindow::CameraUnit::grab_frame(int frame_number) {
  cap.set(cv::CAP_PROP_POS_FRAMES, frame_number);
  cap.read(frame);
  auto detector = cv::xfeatures2d::SURF::create();
  cvtColor( frame, gray, CV_RGB2GRAY );
  cv::blur( gray, gray, cv::Size(5,5));
  detector->detect(gray,keypoints);
  detector->compute(gray,keypoints, descriptors);

  if(parent->ui->show_features_checkbox->checkState() == Qt::CheckState::Checked) {
    drawKeypoints(
          frame,
          keypoints,
          frame,
          cv::Scalar(0,255,0),
          cv::DrawMatchesFlags::DRAW_OVER_OUTIMG );//| cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

  }
}

void StereoWindow::CameraUnit::show() {
  show_image(frame, *bound_label);
}


void StereoWindow::CameraUnit::show_match(int feature_index) {
  cv::KeyPoint keypoint = keypoints.at(feature_index);
  cv::circle(frame, keypoint.pt, 3, cv::Scalar(0, 0, 255));

}

void StereoWindow::on_match_features_checkbox_toggled(bool ) {
  show_frame(get_frame_number());
}
