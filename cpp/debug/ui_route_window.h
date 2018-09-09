/********************************************************************************
** Form generated from reading UI file 'route_window.ui'
**
** Created by: Qt User Interface Compiler version 5.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ROUTE_WINDOW_H
#define UI_ROUTE_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RouteWindow
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QPushButton *folder_picker_button;
    QPushButton *synch_folder_button;
    QSplitter *splitter_3;
    QSplitter *selection_splitter;
    QWidget *layoutWidget;
    QVBoxLayout *track_column;
    QLabel *track_label;
    QListWidget *track_list;
    QWidget *layoutWidget_2;
    QVBoxLayout *route_column;
    QLabel *route_label;
    QListWidget *route_list;
    QWidget *layoutWidget_3;
    QVBoxLayout *run_column;
    QLabel *run_label;
    QTableWidget *run_list;
    QWidget *layoutWidget1;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_3;
    QLabel *run_position_label;
    QSlider *run_position_slider;
    QSplitter *splitter_2;
    QGraphicsView *graphicsView;
    QTableWidget *run_data;

    void setupUi(QDialog *RouteWindow)
    {
        if (RouteWindow->objectName().isEmpty())
            RouteWindow->setObjectName(QStringLiteral("RouteWindow"));
        RouteWindow->setWindowModality(Qt::NonModal);
        RouteWindow->resize(835, 561);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(RouteWindow->sizePolicy().hasHeightForWidth());
        RouteWindow->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QStringLiteral(":/root/car-gui/anaranjado_icon.svg"), QSize(), QIcon::Normal, QIcon::Off);
        RouteWindow->setWindowIcon(icon);
        verticalLayout_2 = new QVBoxLayout(RouteWindow);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(5, 5, 5, 5);
        folder_picker_button = new QPushButton(RouteWindow);
        folder_picker_button->setObjectName(QStringLiteral("folder_picker_button"));

        horizontalLayout->addWidget(folder_picker_button);

        synch_folder_button = new QPushButton(RouteWindow);
        synch_folder_button->setObjectName(QStringLiteral("synch_folder_button"));

        horizontalLayout->addWidget(synch_folder_button);


        verticalLayout_2->addLayout(horizontalLayout);

        splitter_3 = new QSplitter(RouteWindow);
        splitter_3->setObjectName(QStringLiteral("splitter_3"));
        splitter_3->setOrientation(Qt::Vertical);
        selection_splitter = new QSplitter(splitter_3);
        selection_splitter->setObjectName(QStringLiteral("selection_splitter"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(selection_splitter->sizePolicy().hasHeightForWidth());
        selection_splitter->setSizePolicy(sizePolicy1);
        selection_splitter->setMinimumSize(QSize(0, 214));
        selection_splitter->setOrientation(Qt::Horizontal);
        layoutWidget = new QWidget(selection_splitter);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        track_column = new QVBoxLayout(layoutWidget);
        track_column->setObjectName(QStringLiteral("track_column"));
        track_column->setContentsMargins(0, 0, 0, 0);
        track_label = new QLabel(layoutWidget);
        track_label->setObjectName(QStringLiteral("track_label"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(track_label->sizePolicy().hasHeightForWidth());
        track_label->setSizePolicy(sizePolicy2);

        track_column->addWidget(track_label);

        track_list = new QListWidget(layoutWidget);
        track_list->setObjectName(QStringLiteral("track_list"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(track_list->sizePolicy().hasHeightForWidth());
        track_list->setSizePolicy(sizePolicy3);
        track_list->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

        track_column->addWidget(track_list);

        selection_splitter->addWidget(layoutWidget);
        layoutWidget_2 = new QWidget(selection_splitter);
        layoutWidget_2->setObjectName(QStringLiteral("layoutWidget_2"));
        route_column = new QVBoxLayout(layoutWidget_2);
        route_column->setObjectName(QStringLiteral("route_column"));
        route_column->setContentsMargins(0, 0, 0, 0);
        route_label = new QLabel(layoutWidget_2);
        route_label->setObjectName(QStringLiteral("route_label"));
        sizePolicy2.setHeightForWidth(route_label->sizePolicy().hasHeightForWidth());
        route_label->setSizePolicy(sizePolicy2);

        route_column->addWidget(route_label);

        route_list = new QListWidget(layoutWidget_2);
        route_list->setObjectName(QStringLiteral("route_list"));
        sizePolicy3.setHeightForWidth(route_list->sizePolicy().hasHeightForWidth());
        route_list->setSizePolicy(sizePolicy3);
        route_list->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

        route_column->addWidget(route_list);

        selection_splitter->addWidget(layoutWidget_2);
        layoutWidget_3 = new QWidget(selection_splitter);
        layoutWidget_3->setObjectName(QStringLiteral("layoutWidget_3"));
        run_column = new QVBoxLayout(layoutWidget_3);
        run_column->setObjectName(QStringLiteral("run_column"));
        run_column->setContentsMargins(0, 0, 0, 0);
        run_label = new QLabel(layoutWidget_3);
        run_label->setObjectName(QStringLiteral("run_label"));

        run_column->addWidget(run_label);

        run_list = new QTableWidget(layoutWidget_3);
        run_list->setObjectName(QStringLiteral("run_list"));
        QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(run_list->sizePolicy().hasHeightForWidth());
        run_list->setSizePolicy(sizePolicy4);
        run_list->setSelectionMode(QAbstractItemView::ExtendedSelection);
        run_list->setSelectionBehavior(QAbstractItemView::SelectRows);

        run_column->addWidget(run_list);

        selection_splitter->addWidget(layoutWidget_3);
        splitter_3->addWidget(selection_splitter);
        layoutWidget1 = new QWidget(splitter_3);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        verticalLayout = new QVBoxLayout(layoutWidget1);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(-1, 0, -1, -1);
        run_position_label = new QLabel(layoutWidget1);
        run_position_label->setObjectName(QStringLiteral("run_position_label"));
        run_position_label->setMinimumSize(QSize(50, 0));
        run_position_label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(run_position_label);

        run_position_slider = new QSlider(layoutWidget1);
        run_position_slider->setObjectName(QStringLiteral("run_position_slider"));
        run_position_slider->setMinimum(0);
        run_position_slider->setMaximum(100);
        run_position_slider->setValue(25);
        run_position_slider->setOrientation(Qt::Horizontal);

        horizontalLayout_3->addWidget(run_position_slider);


        verticalLayout->addLayout(horizontalLayout_3);

        splitter_2 = new QSplitter(layoutWidget1);
        splitter_2->setObjectName(QStringLiteral("splitter_2"));
        splitter_2->setOrientation(Qt::Horizontal);
        graphicsView = new QGraphicsView(splitter_2);
        graphicsView->setObjectName(QStringLiteral("graphicsView"));
        QSizePolicy sizePolicy5(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(graphicsView->sizePolicy().hasHeightForWidth());
        graphicsView->setSizePolicy(sizePolicy5);
        graphicsView->setMaximumSize(QSize(16777215, 1677777));
        splitter_2->addWidget(graphicsView);
        run_data = new QTableWidget(splitter_2);
        run_data->setObjectName(QStringLiteral("run_data"));
        run_data->setSelectionBehavior(QAbstractItemView::SelectRows);
        splitter_2->addWidget(run_data);

        verticalLayout->addWidget(splitter_2);

        splitter_3->addWidget(layoutWidget1);

        verticalLayout_2->addWidget(splitter_3);


        retranslateUi(RouteWindow);

        QMetaObject::connectSlotsByName(RouteWindow);
    } // setupUi

    void retranslateUi(QDialog *RouteWindow)
    {
        RouteWindow->setWindowTitle(QApplication::translate("RouteWindow", "Race Analysis", Q_NULLPTR));
        folder_picker_button->setText(QApplication::translate("RouteWindow", "pick a folder", Q_NULLPTR));
        synch_folder_button->setText(QApplication::translate("RouteWindow", "synch folder", Q_NULLPTR));
        track_label->setText(QApplication::translate("RouteWindow", "Track", Q_NULLPTR));
        route_label->setText(QApplication::translate("RouteWindow", "Route", Q_NULLPTR));
        run_label->setText(QApplication::translate("RouteWindow", "Run", Q_NULLPTR));
        run_position_label->setText(QApplication::translate("RouteWindow", "0.0", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class RouteWindow: public Ui_RouteWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ROUTE_WINDOW_H
