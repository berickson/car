#include "picker_window.h"
#include "ui_picker_window.h"

PickerWindow::PickerWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PIckerWindow)
{
    ui->setupUi(this);
}

PickerWindow::~PickerWindow()
{
    delete ui;
}

#include "route_window.h"
void PickerWindow::on_routes_button_clicked()
{
    RouteWindow w;
    w.exec();
}

#include "stereo_window.h"
#include "main_window.h"
#include "lidar_window.h"

void PickerWindow::on_camera_button_clicked()
{
    MainWindow w;
    w.show();
    w.exec();
}

void PickerWindow::on_stereo_camera_button_clicked()
{
    StereoWindow w;
    w.exec();
}

void PickerWindow::on_lidar_button_clicked()
{
   LidarWindow w;
   w.exec();
}
