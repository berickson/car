#include "camera_window.h"
#include "ui_camera_window.h"
#include "route_window.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "opencv2/videoio.hpp"
#include "opencv2/calib3d.hpp"
#include <QMessageBox>
#include <QDateTime>
#include <QDir>

#include <string>
#include <sstream>
#include "../json.hpp"

using namespace std;


/*
v4l2-ctl --device=/dev/video1 --list-formats-ext
ioctl: VIDIOC_ENUM_FMT
  Index       : 0
  Type        : Video Capture
  Pixel Format: 'MJPG' (compressed)
  Name        : Motion-JPEG
    Size: Discrete 1920x1080
      Interval: Discrete 0.033s (30.000 fps)
    Size: Discrete 1280x720
      Interval: Discrete 0.017s (60.000 fps)
    Size: Discrete 1024x768
      Interval: Discrete 0.033s (30.000 fps)
    Size: Discrete 640x480
      Interval: Discrete 0.008s (120.101 fps)
    Size: Discrete 800x600
      Interval: Discrete 0.017s (60.000 fps)
    Size: Discrete 1280x1024
      Interval: Discrete 0.033s (30.000 fps)
    Size: Discrete 320x240
      Interval: Discrete 0.008s (120.101 fps)

  Index       : 1
  Type        : Video Capture
  Pixel Format: 'YUYV'
  Name        : YUYV 4:2:2
    Size: Discrete 1920x1080
      Interval: Discrete 0.167s (6.000 fps)
    Size: Discrete 1280x720
      Interval: Discrete 0.111s (9.000 fps)
    Size: Discrete 1024x768
      Interval: Discrete 0.167s (6.000 fps)
    Size: Discrete 640x480
      Interval: Discrete 0.033s (30.000 fps)
    Size: Discrete 800x600
      Interval: Discrete 0.050s (20.000 fps)
    Size: Discrete 1280x1024
      Interval: Discrete 0.167s (6.000 fps)
    Size: Discrete 320x240
      Interval: Discrete 0.033s (30.000 fps)
*/

void CameraWindow::set_camera()
{
  frame_grabber.end_grabbing();

  if(cap.isOpened()) {
    cap.release();
  }

  string device_name = ui->video_device->currentText().toStdString();
  if(QFile(QString::fromStdString(device_name)).exists()) {
    if(!cap.open(device_name)){
      throw (string) "couldn't open camera";
    }
    frame_grabber.begin_grabbing(&cap, "");
  }
}

CameraWindow::CameraWindow(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CameraWindow)
{
  ui->setupUi(this);

  for(auto res : supported_resolutions) {

    stringstream ss;
    ss << res.width() << "_" << res.height();
    ui->resolutions_combo_box->addItem(QString::fromStdString(ss.str()));
  }
  ui->resolutions_combo_box->setCurrentIndex(1);


  set_camera();

  ui->brightness_slider->setValue(cap.get(cv::CAP_PROP_BRIGHTNESS)*100);
  ui->contrast_slider->setValue(cap.get(cv::CAP_PROP_CONTRAST)*100);
  ui->saturation_slider->setValue(cap.get(cv::CAP_PROP_SATURATION)*100);


  timer.setSingleShot(false);
  timer.setInterval(10);
  connect(&timer, SIGNAL(timeout()), this, SLOT(process_one_frame()));
  connect(ui->frames_per_second_slider, SIGNAL(valueChanged(int)), this, SLOT(fps_changed(int)));
  timer.start();

}

CameraWindow::~CameraWindow()
{
  frame_grabber.end_grabbing();
  if(cap.isOpened())
    cap.release();
  delete ui;
}

void CameraWindow::on_actionExit_triggered()
{
  this->close();
}



void CameraWindow::process_one_frame()
{
  ui->cam_frame_count_text->setText(QString::number(frame_grabber.get_frame_count_grabbed()));
  if(!frame_grabber.get_latest_frame(original_frame)) {
    return;
  }
  original_frame.copyTo(frame);

  if(ui->undistort_checkbox->isChecked()) {
    try {
      stringstream ss;
      ss << QDir::homePath().toStdString() + "/car/camera_calibrations.json";
      string path = ss.str();

      std::ifstream json_file(path);
      nlohmann::json calibration_json = nlohmann::json::parse(json_file);

      string camera_name =
          ui->camera_name->currentText().toStdString()
          + "_"
          + ui->resolutions_combo_box->currentText().toStdString();

      string check = calibration_json.dump();

      auto j = calibration_json.find(camera_name.c_str());
      if(j==calibration_json.end())  {
        ui->undistort_checkbox->setChecked(false);
        throw string("could not find camera in json calibration");
      }
      auto m=j->at("camera_matrix");
      cv::Mat intrinsic = (cv::Mat1d(3, 3) <<  m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]);

      auto d = j->at("dist_coefs");
      cv::Mat distortionCoefficients = (cv::Mat1d(1, 5) << d[0][0], d[0][1], d[0][2], d[0][3], d[0][4]);
      cv::Mat undistorted;
      cv::undistort(frame, undistorted, intrinsic, distortionCoefficients);
      undistorted.copyTo(frame);

    } catch (...) {
      ;
    }

  }

  if(ui->flip_checkbox->isChecked()) {
    cv::flip(frame,frame,-1);
  }

  vector<cv::Point2f> corners;
  cv::Size chessboard_size(6,9);
  bool found = cv::findChessboardCorners(frame, chessboard_size, corners);
  cv::drawChessboardCorners(frame, chessboard_size, corners, found);


  if(ui->find_correspondences_checkbox->isChecked()) {
    tracker.process_image(frame);
  }
  ++frame_count;

  ui->frame_count_text->setText(QString::number(frame_count));


  cv::cvtColor(frame,frame,cv::COLOR_BGR2RGB);
  QImage imdisplay((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
  std::stringstream ss;
  ss << tracker.homography;
  ui->homography_text->setText(QString::fromStdString(ss.str()));
  //cv::putText(frame, ss.str(), cv::Point(50,50), cv::FONT_HERSHEY_SIMPLEX, 1, red, 3);

  ui->display_image->setFixedSize(imdisplay.size());
  if(ui->show_image_checkbox->checkState()==Qt::CheckState::Checked) {
    QPixmap pixmap = QPixmap::fromImage(imdisplay);
    ui->display_image->setPixmap(QPixmap::fromImage(imdisplay));
    ui->scroll_area_contents->setBaseSize(imdisplay.size());
  }
}

void CameraWindow::fps_changed(int fps)
{
  timer.setInterval((1./fps)*1000);

}

void CameraWindow::on_routesButton_clicked()
{
    RouteWindow r(this);

    r.exec();
}

void CameraWindow::on_webcamButton_clicked()
{

  std::vector<std::string> prop_names =
        { "CAP_PROP_POS_MSEC",      //0
          "CAP_PROP_POS_FRAMES",    //1
          "CAP_PROP_POS_AVI_RATIO", //2 };
          "CAP_PROP_FRAME_WIDTH",   //3
          "CAP_PROP_FRAME_HEIGHT",  //4
          "CAP_PROP_FPS",           //5
          "CAP_PROP_FOURCC",        //6
          "CAP_PROP_FRAME_COUNT",   //7
          "CAP_PROP_FORMAT",        //8
          "CAP_PROP_MODE",          //9
          "CAP_PROP_BRIGHTNESS",   //10
          "CAP_PROP_CONTRAST",     //11
          "CAP_PROP_SATURATION",   //12
          "CAP_PROP_HUE",          //13
          "CAP_PROP_GAIN",         //14
          "CAP_PROP_EXPOSURE",     //15
          "CAP_PROP_CONVERT_RGB",  //16
          "CAP_PROP_WHITE_BALANCE_BLUE_U",//17
          "CAP_PROP_RECTIFICATION",//18
          "CAP_PROP_MONOCHROME",   //19
          "CAP_PROP_SHARPNESS",    //20
          "CAP_PROP_AUTO_EXPOSURE", //21 DC1394: exposure control done by camera, user can adjust refernce level using this feature
          "CAP_PROP_GAMMA",        //22
          "CAP_PROP_TEMPERATURE",  //23
          "CAP_PROP_TRIGGER",      //24
          "CAP_PROP_TRIGGER_DELAY",//25
          "CAP_PROP_WHITE_BALANCE_RED_V",//26
          "CAP_PROP_ZOOM",         //27
          "CAP_PROP_FOCUS",        //28
          "CAP_PROP_GUID",         //29
          "CAP_PROP_ISO_SPEED",    //30
          "CAP_PROP_BACKLIGHT",    //32
          "CAP_PROP_PAN",          //33
          "CAP_PROP_TILT",         //34
          "CAP_PROP_ROLL",         //35
          "CAP_PROP_IRIS",         //36
          "CAP_PROP_SETTINGS",     //37
          "CAP_PROP_BUFFERSIZE",   //38
          "CAP_PROP_AUTOFOCUS"    //39
        };

  stringstream ss;
  for(int i=0;i<=39;i++) {
    if(i==cv::CAP_PROP_BUFFERSIZE) break; // reading this cause bug?

    ss << "CAP_PROP " << i << ":";
    ss << prop_names[i] << " ";
    try{
      ss << cap.get(i);

    } catch (...) {
      ss << "ERROR";
    }
    ss << endl;
  }
  QMessageBox dialog(this);
  dialog.setText(QString::fromStdString(ss.str()));
  dialog.exec();
}

void CameraWindow::on_brightness_slider_valueChanged(int value)
{
    cap.set(cv::CAP_PROP_BRIGHTNESS,value/100.);
}

void CameraWindow::on_contrast_slider_valueChanged(int value)
{
    cap.set(cv::CAP_PROP_CONTRAST,value/100.);
}

void CameraWindow::on_hue_slider_valueChanged(int value)
{
  cap.set(cv::CAP_PROP_HUE,value/100.);

}

void CameraWindow::on_saturation_slider_valueChanged(int value)
{
  cap.set(cv::CAP_PROP_SATURATION,value/100.);
}


void CameraWindow::on_resolutions_combo_box_currentIndexChanged(int )
{
    QSize resolution = supported_resolutions[ui->resolutions_combo_box->currentIndex()];
    cap.set(cv::CAP_PROP_FRAME_WIDTH, resolution.width());
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, resolution.height());
    ui->scroll_area_contents->setFixedSize(resolution);
    ui->display_image->setFixedSize(resolution);
}

void CameraWindow::on_video_device_currentTextChanged(const QString &)
{
  set_camera();
}

void CameraWindow::on_take_picture_button_clicked()
{
  stringstream ss;
  ss << QDir::homePath().toStdString() + "/car/data/capture_"
       << QDateTime::currentDateTime().toString("yyyy-MM-ddThh.mm.ss.zzz").toStdString()
       << ".png";
  string path = ss.str();

  cv::imwrite(path, original_frame);
}
