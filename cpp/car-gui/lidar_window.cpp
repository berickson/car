#include "lidar_window.h"
#include "ui_lidar_window.h"
#include <QPainter>

LidarWindow::LidarWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LidarWindow)
{
    ui->setupUi(this);
}

LidarWindow::~LidarWindow()
{
    delete ui;
}

void LidarWindow::paintEvent(QPaintEvent *) {
    QPainter painter;
    painter.begin(this);
    QBrush brush;
    QPen pen;
    QColor red(255,0,0);
    brush.setColor(red);
    pen.setColor(red);
    painter.setBrush(brush);
    painter.setPen(pen);
    painter.drawLine(0,0,100,100);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.end();
}
