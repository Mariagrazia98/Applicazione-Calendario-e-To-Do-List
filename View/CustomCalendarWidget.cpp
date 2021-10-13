//
// Created by manue on 09/10/2021.
//

#include <iostream>
#include "CustomCalendarWidget.h"
#include "../Model/calendarevent.h"
#include "../Model/calendartodo.h"

CustomCalendarWidget::CustomCalendarWidget(QWidget *parent) : QCalendarWidget(parent) {
    setLocale(QLocale::English);
    setMinimumDate(QDate(2000, 1, 1));
    setMaximumDate(QDate(2121, 12, 31));
    setGridVisible(true);
}

void CustomCalendarWidget::paintCell(QPainter *painter, const QRect &rect, QDate date) const {
    QCalendarWidget::paintCell(painter, rect, date);
    for (int i = 0; i < calendarObjects.size(); ++i) {
        // TODO: gestire ricorrenze
        if (calendarObjects[i]->getStartDateTime().date() == date) {
            paintDate(painter, rect);
            return;
        }

    }

}

void CustomCalendarWidget::paintDate(QPainter *painter, const QRect &rect) const {
    QPen pen;
    pen.setColor(QColorConstants::Red);
    painter->setPen(pen);
    painter->setOpacity(0.1);
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColorConstants::Red);
    painter->setBrush(brush);
    //painter->drawRect(rect.adjusted(rect.width() - 10, 0, 0, -rect.height() + 10));
    painter->drawRect(rect.adjusted(0, 0, -1, -1));
    painter->setOpacity(1);
}

void CustomCalendarWidget::setCalendarObjects(const QList<CalendarObject *> &calendarObjects) {
    this->calendarObjects.clear();
    CustomCalendarWidget::calendarObjects = calendarObjects;
}
