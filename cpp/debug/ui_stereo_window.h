/********************************************************************************
** Form generated from reading UI file 'stereo_window.ui'
**
** Created by: Qt User Interface Compiler version 5.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STEREO_WINDOW_H
#define UI_STEREO_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_StereoWIndow
{
public:
    QLabel *frame_count_label;
    QLabel *frame_count;
    QSlider *frame_slider;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *left_image;
    QLabel *right_image;
    QCheckBox *show_features_checkbox;
    QCheckBox *match_features_checkbox;
    QCheckBox *undistort_checkbox;
    QLabel *log_output;
    QCheckBox *depth_map_checkbox;

    void setupUi(QDialog *StereoWIndow)
    {
        if (StereoWIndow->objectName().isEmpty())
            StereoWIndow->setObjectName(QStringLiteral("StereoWIndow"));
        StereoWIndow->resize(938, 757);
        StereoWIndow->setBaseSize(QSize(640, 480));
        frame_count_label = new QLabel(StereoWIndow);
        frame_count_label->setObjectName(QStringLiteral("frame_count_label"));
        frame_count_label->setGeometry(QRect(80, 500, 91, 16));
        frame_count = new QLabel(StereoWIndow);
        frame_count->setObjectName(QStringLiteral("frame_count"));
        frame_count->setGeometry(QRect(180, 500, 51, 16));
        frame_slider = new QSlider(StereoWIndow);
        frame_slider->setObjectName(QStringLiteral("frame_slider"));
        frame_slider->setGeometry(QRect(60, 530, 851, 16));
        frame_slider->setOrientation(Qt::Horizontal);
        layoutWidget = new QWidget(StereoWIndow);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(60, 10, 1288, 482));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        left_image = new QLabel(layoutWidget);
        left_image->setObjectName(QStringLiteral("left_image"));
        left_image->setMinimumSize(QSize(640, 480));
        left_image->setBaseSize(QSize(640, 480));

        horizontalLayout->addWidget(left_image);

        right_image = new QLabel(layoutWidget);
        right_image->setObjectName(QStringLiteral("right_image"));
        right_image->setMinimumSize(QSize(640, 480));

        horizontalLayout->addWidget(right_image);

        show_features_checkbox = new QCheckBox(StereoWIndow);
        show_features_checkbox->setObjectName(QStringLiteral("show_features_checkbox"));
        show_features_checkbox->setGeometry(QRect(60, 550, 121, 20));
        show_features_checkbox->setChecked(false);
        match_features_checkbox = new QCheckBox(StereoWIndow);
        match_features_checkbox->setObjectName(QStringLiteral("match_features_checkbox"));
        match_features_checkbox->setGeometry(QRect(180, 550, 131, 20));
        match_features_checkbox->setChecked(false);
        undistort_checkbox = new QCheckBox(StereoWIndow);
        undistort_checkbox->setObjectName(QStringLiteral("undistort_checkbox"));
        undistort_checkbox->setGeometry(QRect(310, 550, 121, 20));
        undistort_checkbox->setChecked(false);
        log_output = new QLabel(StereoWIndow);
        log_output->setObjectName(QStringLiteral("log_output"));
        log_output->setGeometry(QRect(60, 580, 751, 141));
        depth_map_checkbox = new QCheckBox(StereoWIndow);
        depth_map_checkbox->setObjectName(QStringLiteral("depth_map_checkbox"));
        depth_map_checkbox->setGeometry(QRect(400, 550, 131, 20));
        depth_map_checkbox->setChecked(true);
        layoutWidget->raise();
        frame_count_label->raise();
        frame_count->raise();
        frame_slider->raise();
        show_features_checkbox->raise();
        match_features_checkbox->raise();
        undistort_checkbox->raise();
        log_output->raise();
        depth_map_checkbox->raise();

        retranslateUi(StereoWIndow);

        QMetaObject::connectSlotsByName(StereoWIndow);
    } // setupUi

    void retranslateUi(QDialog *StereoWIndow)
    {
        StereoWIndow->setWindowTitle(QApplication::translate("StereoWIndow", "Dialog", Q_NULLPTR));
        frame_count_label->setText(QApplication::translate("StereoWIndow", "Frame Count: ", Q_NULLPTR));
        frame_count->setText(QApplication::translate("StereoWIndow", "#", Q_NULLPTR));
        left_image->setText(QApplication::translate("StereoWIndow", "Left", Q_NULLPTR));
        right_image->setText(QApplication::translate("StereoWIndow", "Right", Q_NULLPTR));
        show_features_checkbox->setText(QApplication::translate("StereoWIndow", "Show Features", Q_NULLPTR));
        match_features_checkbox->setText(QApplication::translate("StereoWIndow", "Match Features", Q_NULLPTR));
        undistort_checkbox->setText(QApplication::translate("StereoWIndow", "undistort", Q_NULLPTR));
        log_output->setText(QApplication::translate("StereoWIndow", "#", Q_NULLPTR));
        depth_map_checkbox->setText(QApplication::translate("StereoWIndow", "Depth Map", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class StereoWIndow: public Ui_StereoWIndow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STEREO_WINDOW_H
