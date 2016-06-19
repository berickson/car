#ifndef CARITEM_H
#define CARITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QWidget>
#include <QPainter>



class CarItem : public QGraphicsItem {
public:
    QRectF boundingRect() const;

    int x = 6;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *,
               QWidget *);

};


class CarHolder : public QObject  {
    Q_OBJECT
public:
    CarItem * car_item;

public slots:
    void slot1();
};


#endif // CARITEM_H
