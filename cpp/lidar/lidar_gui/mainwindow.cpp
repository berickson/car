#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../lidar.h"

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

}

MainWindow::~MainWindow()
{
    lidar.stop();
    delete ui;
}

void MainWindow::process_lidar()
{
    lidar.get_scan();
        ui->scan_count_label->setText(QString::number(lidar.completed_scan_count));
       //ui->lidar_output->setText(QString(lidar.current_scan.display_string().c_str()));


}
