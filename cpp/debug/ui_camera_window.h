/********************************************************************************
** Form generated from reading UI file 'camera_window.ui'
**
** Created by: Qt User Interface Compiler version 5.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CAMERA_WINDOW_H
#define UI_CAMERA_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CameraWindow
{
public:
    QAction *actionExit;
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *verticalLayout;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QComboBox *video_device;
    QComboBox *camera_name;
    QComboBox *video_device_2;
    QComboBox *camera_name_2;
    QComboBox *resolutions_combo_box;
    QPushButton *webcamButton;
    QPushButton *take_picture_button;
    QCheckBox *flip_checkbox;
    QCheckBox *undistort_checkbox;
    QCheckBox *find_correspondences_checkbox;
    QCheckBox *show_image_checkbox;
    QSpacerItem *horizontalSpacer;
    QScrollArea *scrollArea;
    QWidget *scroll_area_contents;
    QHBoxLayout *horizontalLayout_10;
    QHBoxLayout *horizontalLayout_2;
    QLabel *display_image;
    QLabel *display_image_2;
    QGridLayout *gridLayout_4;
    QLabel *homography_label_2;
    QLabel *homography_label_5;
    QLabel *homography_label;
    QSpacerItem *horizontalSpacer_8;
    QSlider *frames_per_second_slider;
    QLabel *homography_label_4;
    QLabel *homography_label_6;
    QLabel *frame_count_label;
    QSlider *saturation_slider;
    QSlider *contrast_slider;
    QLabel *homography_label_3;
    QLabel *homography_text;
    QSlider *brightness_slider;
    QLabel *frame_count_text;
    QSlider *hue_slider;
    QLabel *cam_frame_count_label;
    QLabel *cam_frame_count_text;

    void setupUi(QWidget *CameraWindow)
    {
        if (CameraWindow->objectName().isEmpty())
            CameraWindow->setObjectName(QStringLiteral("CameraWindow"));
        CameraWindow->resize(1291, 812);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(CameraWindow->sizePolicy().hasHeightForWidth());
        CameraWindow->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QStringLiteral("anaranjado_icon.svg"), QSize(), QIcon::Normal, QIcon::Off);
        CameraWindow->setWindowIcon(icon);
        CameraWindow->setProperty("documentMode", QVariant(true));
        actionExit = new QAction(CameraWindow);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        verticalLayout_3 = new QVBoxLayout(CameraWindow);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setSizeConstraint(QLayout::SetNoConstraint);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        video_device = new QComboBox(CameraWindow);
        video_device->setObjectName(QStringLiteral("video_device"));
        video_device->setMinimumSize(QSize(130, 0));

        horizontalLayout->addWidget(video_device);

        camera_name = new QComboBox(CameraWindow);
        camera_name->setObjectName(QStringLiteral("camera_name"));
        camera_name->setMinimumSize(QSize(130, 0));

        horizontalLayout->addWidget(camera_name);

        video_device_2 = new QComboBox(CameraWindow);
        video_device_2->setObjectName(QStringLiteral("video_device_2"));
        video_device_2->setMinimumSize(QSize(130, 0));

        horizontalLayout->addWidget(video_device_2);

        camera_name_2 = new QComboBox(CameraWindow);
        camera_name_2->setObjectName(QStringLiteral("camera_name_2"));
        camera_name_2->setMinimumSize(QSize(130, 0));

        horizontalLayout->addWidget(camera_name_2);

        resolutions_combo_box = new QComboBox(CameraWindow);
        resolutions_combo_box->setObjectName(QStringLiteral("resolutions_combo_box"));

        horizontalLayout->addWidget(resolutions_combo_box);

        webcamButton = new QPushButton(CameraWindow);
        webcamButton->setObjectName(QStringLiteral("webcamButton"));

        horizontalLayout->addWidget(webcamButton);

        take_picture_button = new QPushButton(CameraWindow);
        take_picture_button->setObjectName(QStringLiteral("take_picture_button"));

        horizontalLayout->addWidget(take_picture_button);

        flip_checkbox = new QCheckBox(CameraWindow);
        flip_checkbox->setObjectName(QStringLiteral("flip_checkbox"));
        flip_checkbox->setChecked(true);
        flip_checkbox->setTristate(false);

        horizontalLayout->addWidget(flip_checkbox);

        undistort_checkbox = new QCheckBox(CameraWindow);
        undistort_checkbox->setObjectName(QStringLiteral("undistort_checkbox"));
        undistort_checkbox->setChecked(false);
        undistort_checkbox->setTristate(false);

        horizontalLayout->addWidget(undistort_checkbox);

        find_correspondences_checkbox = new QCheckBox(CameraWindow);
        find_correspondences_checkbox->setObjectName(QStringLiteral("find_correspondences_checkbox"));
        find_correspondences_checkbox->setChecked(false);
        find_correspondences_checkbox->setTristate(false);

        horizontalLayout->addWidget(find_correspondences_checkbox);

        show_image_checkbox = new QCheckBox(CameraWindow);
        show_image_checkbox->setObjectName(QStringLiteral("show_image_checkbox"));
        show_image_checkbox->setChecked(true);
        show_image_checkbox->setTristate(false);

        horizontalLayout->addWidget(show_image_checkbox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout_2->addLayout(horizontalLayout);

        scrollArea = new QScrollArea(CameraWindow);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setWidgetResizable(false);
        scroll_area_contents = new QWidget();
        scroll_area_contents->setObjectName(QStringLiteral("scroll_area_contents"));
        scroll_area_contents->setGeometry(QRect(0, 0, 640, 240));
        horizontalLayout_10 = new QHBoxLayout(scroll_area_contents);
        horizontalLayout_10->setSpacing(0);
        horizontalLayout_10->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        horizontalLayout_10->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setSizeConstraint(QLayout::SetNoConstraint);
        display_image = new QLabel(scroll_area_contents);
        display_image->setObjectName(QStringLiteral("display_image"));
        display_image->setMinimumSize(QSize(320, 240));

        horizontalLayout_2->addWidget(display_image);

        display_image_2 = new QLabel(scroll_area_contents);
        display_image_2->setObjectName(QStringLiteral("display_image_2"));
        display_image_2->setMinimumSize(QSize(320, 240));
        display_image_2->setBaseSize(QSize(320, 240));

        horizontalLayout_2->addWidget(display_image_2);


        horizontalLayout_10->addLayout(horizontalLayout_2);

        scrollArea->setWidget(scroll_area_contents);

        verticalLayout_2->addWidget(scrollArea);

        gridLayout_4 = new QGridLayout();
        gridLayout_4->setSpacing(6);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        homography_label_2 = new QLabel(CameraWindow);
        homography_label_2->setObjectName(QStringLiteral("homography_label_2"));

        gridLayout_4->addWidget(homography_label_2, 5, 0, 1, 1);

        homography_label_5 = new QLabel(CameraWindow);
        homography_label_5->setObjectName(QStringLiteral("homography_label_5"));

        gridLayout_4->addWidget(homography_label_5, 0, 0, 1, 1);

        homography_label = new QLabel(CameraWindow);
        homography_label->setObjectName(QStringLiteral("homography_label"));

        gridLayout_4->addWidget(homography_label, 6, 0, 1, 1);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer_8, 0, 2, 1, 1);

        frames_per_second_slider = new QSlider(CameraWindow);
        frames_per_second_slider->setObjectName(QStringLiteral("frames_per_second_slider"));
        frames_per_second_slider->setMinimum(1);
        frames_per_second_slider->setMaximum(30);
        frames_per_second_slider->setValue(10);
        frames_per_second_slider->setSliderPosition(10);
        frames_per_second_slider->setOrientation(Qt::Horizontal);
        frames_per_second_slider->setTickPosition(QSlider::TicksBelow);
        frames_per_second_slider->setTickInterval(1);

        gridLayout_4->addWidget(frames_per_second_slider, 5, 1, 1, 1);

        homography_label_4 = new QLabel(CameraWindow);
        homography_label_4->setObjectName(QStringLiteral("homography_label_4"));

        gridLayout_4->addWidget(homography_label_4, 3, 0, 1, 1);

        homography_label_6 = new QLabel(CameraWindow);
        homography_label_6->setObjectName(QStringLiteral("homography_label_6"));

        gridLayout_4->addWidget(homography_label_6, 2, 0, 1, 1);

        frame_count_label = new QLabel(CameraWindow);
        frame_count_label->setObjectName(QStringLiteral("frame_count_label"));

        gridLayout_4->addWidget(frame_count_label, 7, 0, 1, 1);

        saturation_slider = new QSlider(CameraWindow);
        saturation_slider->setObjectName(QStringLiteral("saturation_slider"));
        saturation_slider->setMinimum(0);
        saturation_slider->setMaximum(100);
        saturation_slider->setValue(10);
        saturation_slider->setSliderPosition(10);
        saturation_slider->setOrientation(Qt::Horizontal);
        saturation_slider->setTickPosition(QSlider::TicksBelow);
        saturation_slider->setTickInterval(10);

        gridLayout_4->addWidget(saturation_slider, 2, 1, 1, 1);

        contrast_slider = new QSlider(CameraWindow);
        contrast_slider->setObjectName(QStringLiteral("contrast_slider"));
        contrast_slider->setMinimum(0);
        contrast_slider->setMaximum(100);
        contrast_slider->setValue(10);
        contrast_slider->setSliderPosition(10);
        contrast_slider->setOrientation(Qt::Horizontal);
        contrast_slider->setTickPosition(QSlider::TicksBelow);
        contrast_slider->setTickInterval(10);

        gridLayout_4->addWidget(contrast_slider, 3, 1, 1, 1);

        homography_label_3 = new QLabel(CameraWindow);
        homography_label_3->setObjectName(QStringLiteral("homography_label_3"));

        gridLayout_4->addWidget(homography_label_3, 4, 0, 1, 1);

        homography_text = new QLabel(CameraWindow);
        homography_text->setObjectName(QStringLiteral("homography_text"));

        gridLayout_4->addWidget(homography_text, 6, 1, 1, 2);

        brightness_slider = new QSlider(CameraWindow);
        brightness_slider->setObjectName(QStringLiteral("brightness_slider"));
        brightness_slider->setMinimum(0);
        brightness_slider->setMaximum(100);
        brightness_slider->setValue(10);
        brightness_slider->setSliderPosition(10);
        brightness_slider->setOrientation(Qt::Horizontal);
        brightness_slider->setTickPosition(QSlider::TicksBelow);
        brightness_slider->setTickInterval(10);

        gridLayout_4->addWidget(brightness_slider, 4, 1, 1, 1);

        frame_count_text = new QLabel(CameraWindow);
        frame_count_text->setObjectName(QStringLiteral("frame_count_text"));

        gridLayout_4->addWidget(frame_count_text, 7, 1, 1, 1);

        hue_slider = new QSlider(CameraWindow);
        hue_slider->setObjectName(QStringLiteral("hue_slider"));
        hue_slider->setMinimum(0);
        hue_slider->setMaximum(100);
        hue_slider->setValue(10);
        hue_slider->setSliderPosition(10);
        hue_slider->setOrientation(Qt::Horizontal);
        hue_slider->setTickPosition(QSlider::TicksBelow);
        hue_slider->setTickInterval(10);

        gridLayout_4->addWidget(hue_slider, 0, 1, 1, 1);

        cam_frame_count_label = new QLabel(CameraWindow);
        cam_frame_count_label->setObjectName(QStringLiteral("cam_frame_count_label"));

        gridLayout_4->addWidget(cam_frame_count_label, 8, 0, 1, 1);

        cam_frame_count_text = new QLabel(CameraWindow);
        cam_frame_count_text->setObjectName(QStringLiteral("cam_frame_count_text"));

        gridLayout_4->addWidget(cam_frame_count_text, 8, 1, 1, 1);


        verticalLayout_2->addLayout(gridLayout_4);


        verticalLayout->addLayout(verticalLayout_2);


        verticalLayout_3->addLayout(verticalLayout);


        retranslateUi(CameraWindow);

        QMetaObject::connectSlotsByName(CameraWindow);
    } // setupUi

    void retranslateUi(QWidget *CameraWindow)
    {
        CameraWindow->setWindowTitle(QApplication::translate("CameraWindow", "Orange Crash", Q_NULLPTR));
        actionExit->setText(QApplication::translate("CameraWindow", "Exit", Q_NULLPTR));
#ifndef QT_NO_SHORTCUT
        actionExit->setShortcut(QApplication::translate("CameraWindow", "Ctrl+Q", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        video_device->clear();
        video_device->insertItems(0, QStringList()
         << QString()
         << QApplication::translate("CameraWindow", "/dev/video0", Q_NULLPTR)
         << QApplication::translate("CameraWindow", "/dev/video1", Q_NULLPTR)
         << QApplication::translate("CameraWindow", "/dev/video2", Q_NULLPTR)
         << QApplication::translate("CameraWindow", "/dev/video3", Q_NULLPTR)
        );
        camera_name->clear();
        camera_name->insertItems(0, QStringList()
         << QString()
         << QApplication::translate("CameraWindow", "elp1_left", Q_NULLPTR)
         << QApplication::translate("CameraWindow", "elp1_right", Q_NULLPTR)
         << QApplication::translate("CameraWindow", "elp2_left", Q_NULLPTR)
         << QApplication::translate("CameraWindow", "elp2_right", Q_NULLPTR)
        );
        video_device_2->clear();
        video_device_2->insertItems(0, QStringList()
         << QString()
         << QApplication::translate("CameraWindow", "/dev/video0", Q_NULLPTR)
         << QApplication::translate("CameraWindow", "/dev/video1", Q_NULLPTR)
         << QApplication::translate("CameraWindow", "/dev/video2", Q_NULLPTR)
         << QApplication::translate("CameraWindow", "/dev/video3", Q_NULLPTR)
        );
        camera_name_2->clear();
        camera_name_2->insertItems(0, QStringList()
         << QString()
         << QApplication::translate("CameraWindow", "elp1_left", Q_NULLPTR)
         << QApplication::translate("CameraWindow", "elp1_right", Q_NULLPTR)
         << QApplication::translate("CameraWindow", "elp2_left", Q_NULLPTR)
         << QApplication::translate("CameraWindow", "elp2_right", Q_NULLPTR)
        );
        webcamButton->setText(QApplication::translate("CameraWindow", "cam info", Q_NULLPTR));
        take_picture_button->setText(QApplication::translate("CameraWindow", "take picture", Q_NULLPTR));
        flip_checkbox->setText(QApplication::translate("CameraWindow", "Flip", Q_NULLPTR));
        undistort_checkbox->setText(QApplication::translate("CameraWindow", "Undistort", Q_NULLPTR));
        find_correspondences_checkbox->setText(QApplication::translate("CameraWindow", "Find Correspondences", Q_NULLPTR));
        show_image_checkbox->setText(QApplication::translate("CameraWindow", "Show Image", Q_NULLPTR));
        display_image->setText(QApplication::translate("CameraWindow", "image 1 here", Q_NULLPTR));
        display_image_2->setText(QApplication::translate("CameraWindow", "image2 here", Q_NULLPTR));
        homography_label_2->setText(QApplication::translate("CameraWindow", "Frames per Second:", Q_NULLPTR));
        homography_label_5->setText(QApplication::translate("CameraWindow", "Hue", Q_NULLPTR));
        homography_label->setText(QApplication::translate("CameraWindow", "Homography", Q_NULLPTR));
        homography_label_4->setText(QApplication::translate("CameraWindow", "Contrast", Q_NULLPTR));
        homography_label_6->setText(QApplication::translate("CameraWindow", "Saturation", Q_NULLPTR));
        frame_count_label->setText(QApplication::translate("CameraWindow", "Frame Count:", Q_NULLPTR));
        homography_label_3->setText(QApplication::translate("CameraWindow", "Brightness", Q_NULLPTR));
        homography_text->setText(QApplication::translate("CameraWindow", "[]", Q_NULLPTR));
        frame_count_text->setText(QApplication::translate("CameraWindow", "0", Q_NULLPTR));
        cam_frame_count_label->setText(QApplication::translate("CameraWindow", "Cam Frame Count:", Q_NULLPTR));
        cam_frame_count_text->setText(QApplication::translate("CameraWindow", "0", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class CameraWindow: public Ui_CameraWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CAMERA_WINDOW_H
