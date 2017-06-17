#ifndef LIDAR_WINDOW_H
#define LIDAR_WINDOW_H

#include <QDialog>

namespace Ui {
class LidarWindow;
}

class LidarWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LidarWindow(QWidget *parent = 0);
    ~LidarWindow();
   void paintEvent(QPaintEvent *event);
private:
    Ui::LidarWindow *ui;
};

#endif // LIDAR_WINDOW_H
