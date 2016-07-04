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
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  cap.set(cv::CAP_PROP_FRAME_WIDTH,320);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT,240);

  if(!cap.open(0)) throw (string) "couldn't open camera";

  connect(&timer, SIGNAL(timeout()), this, SLOT(on_pushButton_clicked()));
  timer.start();

}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}



void MainWindow::on_pushButton_clicked()
{
  int frame_count = 0;

  cv::Mat frame;

  //Tracker tracker;

  for(;;) {

    cap >> frame;
    //tracker.process_image(frame);
    ++frame_count;
    const cv::Scalar white = cv::Scalar(255,255,255);
    const cv::Scalar green = cv::Scalar(0,255,0);
    const cv::Scalar red = cv::Scalar(0,0,255);

    cv::putText(frame, (string) to_string(frame_count), cv::Point(50,50), cv::FONT_HERSHEY_SIMPLEX, 1, red, 3);

    cv::cvtColor(frame,frame,cv::COLOR_BGR2RGB);
    QImage imdisplay((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    ui->display_image->setPixmap(QPixmap::fromImage(imdisplay));
    break;


    //putText(frame, (string) to_string(tracker.low_distance), cv::Point(50,100), cv::FONT_HERSHEY_SIMPLEX ,1, red, 3);
    //putText(frame, (string) to_string(tracker.high_distance), cv::Point(50,150), cv::FONT_HERSHEY_SIMPLEX ,1, red, 3);

    //cv::imshow("window 1", frame);
    //if(cv::waitKey(1)==27) break;
  }

}

void MainWindow::on_routesButton_clicked()
{
    RouteWindow r(this);

    r.exec();
}
