#ifndef PICKER_WINDOW_H
#define PICKER_WINDOW_H

#include <QDialog>

namespace Ui {
class PIckerWindow;
}

class PickerWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PickerWindow(QWidget *parent = 0);
    ~PickerWindow();

private slots:
    void on_routes_button_clicked();

    void on_camera_button_clicked();

    void on_stereo_camera_button_clicked();

    void on_lidar_button_clicked();

private:
    Ui::PIckerWindow *ui;
};

#endif // PICKER_WINDOW_H
