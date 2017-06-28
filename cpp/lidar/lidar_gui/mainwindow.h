#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "QGraphicsScene"
#include "../lidar.h"
#include <fstream>

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

    void on_record_button_clicked();

private:
    std::fstream out_file;

    bool is_recording = false;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
