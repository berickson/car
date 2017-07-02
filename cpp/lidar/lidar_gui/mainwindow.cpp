#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../lidar.h"
#include "QGraphicsView"
#include "QGraphicsScene"
#include "QFileDialog"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer.setSingleShot(false);
    timer.setInterval(10);
    connect(&timer, SIGNAL(timeout()), this, SLOT(process_lidar()));
    lidar.run();
    timer.start();
    ui->graphics_view->setScene(&scene);
    //ui->graphics_view->scale(100,100);

}

MainWindow::~MainWindow()
{
    lidar.stop();
    delete ui;
}


#include <eigen3/Eigen/Dense>
QPointF world_to_screen(Eigen::Vector2d w) {
    Eigen::Transform<double, 2, Eigen::Affine> t;
    t = t.Identity();
    t.scale(Eigen::Vector2d(-50,50));
    t.translate(Eigen::Vector2d(25,0));
    t.rotate(M_PI/2.);

    Eigen::Vector2d s = t * w;
    return QPointF(s[0], s[1]);
}

void MainWindow::process_lidar()
{
    if(lidar.try_get_scan(1)) {
        ui->scan_count_label->setText(QString::number(lidar.completed_scan_count));
        ui->lidar_output->setText(QString(lidar.current_scan.display_string().c_str()));
        if(is_recording) {
          out_file << lidar.get_scan_csv();
        }
        scene.clear();
        QPen blue;
        QPen red;
        QPen light_gray;
        blue.setColor(QColor(0,0,255,50));
        red.setColor(QColor(255,0,0,50));
        red.setWidth(4);
        light_gray.setColor(QColor(230,230,230));
        // draw 1m to 5m circles
        for(int i=1; i <=5; ++i) {
          scene.addEllipse(QRectF(world_to_screen({-i,-i}),world_to_screen({i,i})),
                           light_gray);
        }
        Eigen::Vector2d world_origin(0,0);
        QPointF screen_origin=world_to_screen(world_origin);
        for(LidarMeasurement & m : lidar.current_scan.measurements) {
            if(m.status == LidarMeasurement::measure_status::ok) {
                QPointF sp = world_to_screen(m.get_point());
                float r = 3;
                QRectF rect;
                rect.setTopLeft(sp);
                rect.setBottomRight(sp);
                rect.adjust(-r,-r,r,r);
                scene.addEllipse(rect, light_gray);

                scene.addLine(QLineF(screen_origin, sp), blue);
            }
        }
        vector<LidarScan::ScanSegment> found_lines = lidar.current_scan.find_lines(0.02);
        for(LidarScan::ScanSegment & found_line : found_lines) {
          QPointF p1 = world_to_screen(lidar.current_scan.measurements[found_line.begin_index].get_point());
          QPointF p2 = world_to_screen(lidar.current_scan.measurements[found_line.end_index].get_point());
          scene.addLine(QLineF(p1, p2), red);
        }
    }
}

void MainWindow::on_record_button_clicked()
{
  if(is_recording) {
    out_file.close();
    ui->record_button->setText("record");
    is_recording = false;

  } else {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.selectFile("recording.csv");
    dialog.show();
    if(dialog.exec()) {
      if(dialog.selectedFiles().length() == 1) {
        std::string filename = dialog.selectedFiles()[0].toStdString();
        out_file.open(filename, fstream::out);
        if(out_file.is_open()) {
          ui->record_button->setText("stop");
          is_recording = true;
          out_file << lidar.get_scan_csv_header();
        }
      }
    }
  }
}
