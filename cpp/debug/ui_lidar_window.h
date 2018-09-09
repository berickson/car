/********************************************************************************
** Form generated from reading UI file 'lidar_window.ui'
**
** Created by: Qt User Interface Compiler version 5.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LIDAR_WINDOW_H
#define UI_LIDAR_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_LidarWindow
{
public:
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *LidarWindow)
    {
        if (LidarWindow->objectName().isEmpty())
            LidarWindow->setObjectName(QStringLiteral("LidarWindow"));
        LidarWindow->resize(562, 402);
        buttonBox = new QDialogButtonBox(LidarWindow);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(210, 370, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        retranslateUi(LidarWindow);
        QObject::connect(buttonBox, SIGNAL(accepted()), LidarWindow, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), LidarWindow, SLOT(reject()));

        QMetaObject::connectSlotsByName(LidarWindow);
    } // setupUi

    void retranslateUi(QDialog *LidarWindow)
    {
        LidarWindow->setWindowTitle(QApplication::translate("LidarWindow", "Dialog", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class LidarWindow: public Ui_LidarWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LIDAR_WINDOW_H
