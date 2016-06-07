#include "main_window.h"
#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QTimer>
#include <QObject>

#include <random>
#include "car_item.h"

using namespace std;



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();

    QGraphicsScene scene;
    scene.addText("hello");
    CarHolder car_holder;
    car_holder.car_item = new CarItem();
    scene.addItem(car_holder.car_item);


    QGraphicsView view(&scene);
    view.setRenderHint(QPainter::Antialiasing, true);
    view.show();

    QTimer * t = new QTimer();
    //QObject::connect(t, SIGNAL(timeout()),car,SLOT(slot1()));
    QObject::connect(t, &QTimer::timeout,&car_holder,&CarHolder::slot1);
    QObject::connect(t, &QTimer::timeout,

                [&]() {
                   view.rotate(2);
                });



    t->start(1000/30);

    return a.exec();
}
