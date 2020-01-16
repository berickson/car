#include "picker-window.h"
#include "ui-picker-window.h"

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

#include "route-window.h"
void PickerWindow::on_routes_button_clicked()
{
    RouteWindow w;
    w.exec();
}

#include "stereo-window.h"
#include "camera-window.h"
#include "lidar-window.h"

void PickerWindow::on_camera_button_clicked()
{
    CameraWindow w;
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
