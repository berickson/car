#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../lidar.h"
#include "QGraphicsView"
#include "QGraphicsScene"

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


QPointF world_to_screen(QPointF w) {
    float scale = 100;
    float offset = 50;
    QPointF s(w.x() * scale, offset - w.y() * scale);
    return s;

}

void MainWindow::process_lidar()
{
    if(lidar.try_get_scan(1)) {
        ui->scan_count_label->setText(QString::number(lidar.completed_scan_count));
        ui->lidar_output->setText(QString(lidar.current_scan.display_string().c_str()));
        scene.clear();
        QPen blue;
        blue.setColor(QColor(0,0,255,50));
        for(LidarMeasurement & m : lidar.current_scan.measurements) {
            if(m.status == LidarMeasurement::measure_status::ok) {
                QPointF wp;
                wp.setX(cos(m.angle - Angle::degrees(90)) * m.distance_meters);
                wp.setY(sin(m.angle - Angle::degrees(90)) * m.distance_meters);
                QPointF sp = world_to_screen(wp);
                QPointF world_origin(0,0);
                QPointF screen_origin=world_to_screen(world_origin);
                float r = 3;
                QRectF rect;
                rect.setTopLeft(sp);
                rect.setBottomRight(sp);
                rect.adjust(-r,-r,r,r);
                QPen pen;
                QBrush brush;
                scene.addEllipse(rect);

                scene.addLine(QLineF(screen_origin, sp), blue);
            }
        }
    }
}
