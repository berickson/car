#include "main_window.h"
#include "ui_main_window.h"
#include "route_window.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "opencv2/videoio.hpp"
#include "opencv2/calib3d.hpp"

#include <string>
#include <sstream>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  cap.set(cv::CAP_PROP_FRAME_WIDTH,320);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT,240);

  if(!cap.open(0)) throw (string) "couldn't open camera";
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
  ++frame_count;
  //const cv::Scalar white = cv::Scalar(255,255,255);
  //const cv::Scalar green = cv::Scalar(0,255,0);
  //const cv::Scalar red = cv::Scalar(0,0,255);

  //cv::putText(frame, (string) to_string(frame_count), cv::Point(50,50), cv::FONT_HERSHEY_SIMPLEX, 1, red, 3);
  ui->frame_count_text->setText(QString::number(frame_count));


  cv::cvtColor(frame,frame,cv::COLOR_BGR2RGB);
  QImage imdisplay((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
  std::stringstream ss;
  ss << tracker.homography;
  ui->homography_text->setText(QString::fromStdString(ss.str()));
  //cv::putText(frame, ss.str(), cv::Point(50,50), cv::FONT_HERSHEY_SIMPLEX, 1, red, 3);


  //putText(frame, (string) to_string(tracker.low_distance), cv::Point(50,100), cv::FONT_HERSHEY_SIMPLEX ,1, red, 3);
  //putText(frame, (string) to_string(tracker.high_distance), cv::Point(50,150), cv::FONT_HERSHEY_SIMPLEX ,1, red, 3);

//  ui->display_image->setFixedSize(imdisplay.size());
  ui->display_image->setPixmap(QPixmap::fromImage(imdisplay));

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
