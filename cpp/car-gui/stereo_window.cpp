#include "stereo_window.h"
#include "ui_stereo_window.h"


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

  cap_l.open("/home/brian/Desktop/Dropbox/car/tracks/avc/routes/R/runs/3/video_left.avi");
  cap_r.open("/home/brian/Desktop/Dropbox/car/tracks/avc/routes/R/runs/3/video_right.avi");
  int frame_count = (int) cap_l.get(cv::CAP_PROP_FRAME_COUNT);
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
  cap_l.set(cv::CAP_PROP_POS_FRAMES, number);
  cap_r.set(cv::CAP_PROP_POS_FRAMES, number);
  cap_l.read(left_frame);
  cap_r.read(right_frame);
  ui->frame_slider->setValue(number);
  show_image(left_frame, *ui->left_image);
  show_image(right_frame, *ui->right_image);

}
