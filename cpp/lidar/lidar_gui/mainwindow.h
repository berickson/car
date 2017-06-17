#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "QGraphicsScene"
#include "../lidar.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    LidarUnit lidar;
    QTimer timer;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QGraphicsScene scene;

private slots:
    void process_lidar();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
