#include "car_item.h"


QRectF CarItem::boundingRect() const {
    qreal penWidth = 1;
    return QRectF(-10 - penWidth / 2, -10 - penWidth / 2,
                  20 + penWidth, 20 + penWidth);
}

void CarItem::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *,
           QWidget *) {
    painter->drawRoundedRect(-10, -10, x%20, 20, 5, 5);
}



void CarHolder::slot1() {
    ++(car_item->x);
    car_item->update();
}
