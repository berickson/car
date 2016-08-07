#include "main_window.h"
#include "ui_main_window.h"
#include "route_window.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "opencv2/videoio.hpp"
#include "opencv2/calib3d.hpp"
#include <QMessageBox>

#include <string>
#include <sstream>
using namespace std;


/*
v4l2-ctl --list-formats-ext
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

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  for(auto res : supported_resolutions) {

    stringstream ss;
    ss << res.width() << " x " << res.height();
    ui->resolutions_combo_box->addItem(QString::fromStdString(ss.str()));
  }
  ui->resolutions_combo_box->setCurrentIndex(1);


  if(!cap.open(0)) throw (string) "couldn't open camera";
  //cap.set(cv::CAP_PROP_EXPOSURE, 0);
  //cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 1);
  //cap.set(cv::CAP_PROP_EXPOSURE, .50);
  //cap.set(cv::CAP_PROP_BRIGHTNESS, .8);
  //cap.set(cv::CAP_PROP_CONTRAST, .63);
  //cap.set(cv::CAP_PROP_SATURATION, .66);
  //cap.set(cv::CAP_PROP_FOURCC, CV_FOURCC('M','J','P','G'));
  //cap.set(cv::CAP_PROP_FPS,120.001);
  cap.set(cv::CAP_PROP_FRAME_WIDTH,640);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT,480);

  ui->brightness_slider->setValue(cap.get(cv::CAP_PROP_BRIGHTNESS)*100);
  ui->contrast_slider->setValue(cap.get(cv::CAP_PROP_CONTRAST)*100);
  ui->saturation_slider->setValue(cap.get(cv::CAP_PROP_SATURATION)*100);
  ui->hue_slider->setValue(cap.get(cv::CAP_PROP_HUE)*100);

  frame_grabber.begin_grabbing(&cap);

  timer.setSingleShot(false);
  timer.setInterval(100);
  connect(&timer, SIGNAL(timeout()), this, SLOT(process_one_frame()));
  connect(ui->frames_per_second_slider, SIGNAL(valueChanged(int)), this, SLOT(fps_changed(int)));
  timer.start();

}

MainWindow::~MainWindow()
{
  frame_grabber.end_grabbing();
  delete ui;
}

void MainWindow::on_actionExit_triggered()
{
  this->close();
}



void MainWindow::process_one_frame()
{
  ui->cam_frame_count_text->setText(QString::number(frame_grabber.get_frame_count_grabbed()));
  if(!frame_grabber.get_latest_frame(frame))
    return;

  tracker.process_image(frame);


  // below calibration matrix is for 640 x 480 ELP mono camera
  cv::Mat intrinsic = (cv::Mat1d(3, 3) <<  5.2212093413002049e+02, 0., 3.2542419118701633e+02, 0.,
      5.2212093413002049e+02, 2.5473325838824445e+02, 0., 0., 1. );

  cv::Mat distortionCoefficients = (cv::Mat1d(1, 5) << -4.0852862075720786e-01, 2.4086510135654188e-01,
                                    -9.7223521589240465e-04, 1.7209244108669266e-04,
                                    -8.6751876741832365e-02);


  cv::undistort(frame,view,intrinsic,distortionCoefficients);



  ++frame_count;
  //const cv::Scalar white = cv::Scalar(255,255,255);
  //const cv::Scalar green = cv::Scalar(0,255,0);
  //const cv::Scalar red = cv::Scalar(0,0,255);

  //cv::putText(frame, (string) to_string(frame_count), cv::Point(50,50), cv::FONT_HERSHEY_SIMPLEX, 1, red, 3);
  ui->frame_count_text->setText(QString::number(frame_count));


  cv::cvtColor(view,view,cv::COLOR_BGR2RGB);
  QImage imdisplay((uchar*)view.data, view.cols, view.rows, view.step, QImage::Format_RGB888);
  std::stringstream ss;
  ss << tracker.homography;
  ui->homography_text->setText(QString::fromStdString(ss.str()));
  //cv::putText(frame, ss.str(), cv::Point(50,50), cv::FONT_HERSHEY_SIMPLEX, 1, red, 3);


  //putText(frame, (string) to_string(tracker.low_distance), cv::Point(50,100), cv::FONT_HERSHEY_SIMPLEX ,1, red, 3);
  //putText(frame, (string) to_string(tracker.high_distance), cv::Point(50,150), cv::FONT_HERSHEY_SIMPLEX ,1, red, 3);

  ui->display_image->setFixedSize(imdisplay.size());
  if(ui->show_image_checkbox->checkState()==Qt::CheckState::Checked) {
    QPixmap pixmap = QPixmap::fromImage(imdisplay);
    ui->display_image->setPixmap(QPixmap::fromImage(imdisplay));
    ui->scroll_area_contents->setBaseSize(imdisplay.size());
  }

}

void MainWindow::fps_changed(int fps)
{
  timer.setInterval((1./fps)*1000);

}

void MainWindow::on_routesButton_clicked()
{
    RouteWindow r(this);

    r.exec();
}

void MainWindow::on_webcamButton_clicked()
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

void MainWindow::on_brightness_slider_valueChanged(int value)
{
    cap.set(cv::CAP_PROP_BRIGHTNESS,value/100.);
}

void MainWindow::on_contrast_slider_valueChanged(int value)
{
    cap.set(cv::CAP_PROP_CONTRAST,value/100.);
}

void MainWindow::on_hue_slider_valueChanged(int value)
{
  cap.set(cv::CAP_PROP_HUE,value/100.);

}

void MainWindow::on_saturation_slider_valueChanged(int value)
{
  cap.set(cv::CAP_PROP_SATURATION,value/100.);
}

void MainWindow::on_resolutions_combo_box_currentIndexChanged()
{
  QSize resolution = supported_resolutions[ui->resolutions_combo_box->currentIndex()];
  cap.set(cv::CAP_PROP_FRAME_WIDTH, resolution.width());
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, resolution.height());
  ui->scroll_area_contents->setMinimumSize(resolution);
  ui->display_image->setMinimumSize(resolution);

}
