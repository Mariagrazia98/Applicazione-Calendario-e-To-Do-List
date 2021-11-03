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
    for (int i = 0; i < calendarObjects->size(); ++i) {
        std::shared_ptr<CalendarEvent> calendarEvent = std::shared_ptr<CalendarEvent>(dynamic_cast<CalendarEvent *>((*calendarObjects)[i].get()));
        if (calendarEvent.get()!= nullptr) {
            if (calendarEvent->getStartDateTime().date() <= date && calendarEvent->getEndDateTime().date() >= date) {
                paintDate(painter, rect);
                return;
            }
        }
        if ((*calendarObjects)[i]->getStartDateTime().date() == date) {
            paintDate(painter, rect);
            return;
        } else if ((*calendarObjects)[i]->getNumRepetition() > 0 && (*calendarObjects)[i]->getUntilDateRipetition() >= date) {
            if (!(*calendarObjects)[i]->getExDates().contains(date)) {
                QDate start = (*calendarObjects)[i]->getStartDateTime().date();
                while (start < date) {
                    switch ((*calendarObjects)[i]->getTypeRepetition()) {
                        case 1:
                            start = start.addDays((*calendarObjects)[i]->getNumRepetition());
                            break;
                        case 2:
                            start = start.addDays((*calendarObjects)[i]->getNumRepetition() * 7);
                            break;
                        case 3:
                            // Monthly
                            start = start.addMonths((*calendarObjects)[i]->getNumRepetition());
                            break;
                        case 4:
                            start = start.addYears((*calendarObjects)[i]->getNumRepetition());
                            break;
                        default:
                            break;
                    }
                }
                if (start == date) {
                    paintDate(painter, rect);
                    return;
                }

            }
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

void CustomCalendarWidget::setCalendarObjects( QList<std::shared_ptr<CalendarObject>>* calendarObjects) {
    this->calendarObjects->clear();
    this->calendarObjects = calendarObjects;
    updateCells();
}
