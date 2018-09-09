/********************************************************************************
** Form generated from reading UI file 'picker_window.ui'
**
** Created by: Qt User Interface Compiler version 5.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PICKER_WINDOW_H
#define UI_PICKER_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_PIckerWindow
{
public:
    QDialogButtonBox *buttonBox;
    QPushButton *routes_button;
    QPushButton *lidar_button;
    QPushButton *camera_button;
    QPushButton *stereo_camera_button;

    void setupUi(QDialog *PIckerWindow)
    {
        if (PIckerWindow->objectName().isEmpty())
            PIckerWindow->setObjectName(QStringLiteral("PIckerWindow"));
        PIckerWindow->resize(400, 300);
        buttonBox = new QDialogButtonBox(PIckerWindow);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        routes_button = new QPushButton(PIckerWindow);
        routes_button->setObjectName(QStringLiteral("routes_button"));
        routes_button->setGeometry(QRect(100, 80, 121, 25));
        lidar_button = new QPushButton(PIckerWindow);
        lidar_button->setObjectName(QStringLiteral("lidar_button"));
        lidar_button->setGeometry(QRect(100, 110, 121, 25));
        camera_button = new QPushButton(PIckerWindow);
        camera_button->setObjectName(QStringLiteral("camera_button"));
        camera_button->setGeometry(QRect(100, 140, 121, 25));
        stereo_camera_button = new QPushButton(PIckerWindow);
        stereo_camera_button->setObjectName(QStringLiteral("stereo_camera_button"));
        stereo_camera_button->setGeometry(QRect(100, 170, 121, 25));

        retranslateUi(PIckerWindow);
        QObject::connect(buttonBox, SIGNAL(accepted()), PIckerWindow, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), PIckerWindow, SLOT(reject()));

        QMetaObject::connectSlotsByName(PIckerWindow);
    } // setupUi

    void retranslateUi(QDialog *PIckerWindow)
    {
        PIckerWindow->setWindowTitle(QApplication::translate("PIckerWindow", "Dialog", Q_NULLPTR));
        routes_button->setText(QApplication::translate("PIckerWindow", "Routes", Q_NULLPTR));
        lidar_button->setText(QApplication::translate("PIckerWindow", "Lidar", Q_NULLPTR));
        camera_button->setText(QApplication::translate("PIckerWindow", "Camera", Q_NULLPTR));
        stereo_camera_button->setText(QApplication::translate("PIckerWindow", "Stereo Camera", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class PIckerWindow: public Ui_PIckerWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PICKER_WINDOW_H
