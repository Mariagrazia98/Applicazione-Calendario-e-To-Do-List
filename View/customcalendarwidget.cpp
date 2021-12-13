//
// Created by manuel on 09/10/2021.
//

#include <iostream>
#include <utility>
#include <execution>
#include "customcalendarwidget.h"
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
    auto result = std::find_if(std::execution::par, calendarObjects.constBegin(), calendarObjects.constEnd(),
                               [date](auto calendarObject) {
                                   if (calendarObject->getExDates().contains(date)) {
                                       return false;
                                   }
                                   std::shared_ptr<CalendarEvent> calendarEvent = std::dynamic_pointer_cast<CalendarEvent>(
                                           calendarObject);
                                   if (calendarEvent) {
                                       if (calendarEvent->getStartDateTime().date() <= date &&
                                           calendarEvent->getEndDateTime().date() >= date) {
                                           return true;
                                       } else if (calendarEvent->getTypeRepetition() !=
                                                  CalendarObject::RepetitionType::NONE &&
                                                  calendarEvent->getNumRepetition() > 0) {
                                           QDateTime start = calendarEvent->getStartDateTime();
                                           QDateTime end = calendarEvent->getEndDateTime();
                                           while (start.date() < date &&
                                                  start.date() <= calendarEvent->getUntilDateRepetition()) {
                                               // if the event takes more than one day I have to readjust the dates
                                               if (start.date() != end.date()) {
                                                   qint64 diff = start.daysTo(end);
                                                   start = start.addDays(diff);
                                                   end = end.addDays(diff);
                                               }
                                               switch (calendarEvent->getTypeRepetition()) {
                                                   case CalendarObject::RepetitionType::DAILY: {
                                                       start = start.addDays(calendarEvent->getNumRepetition());
                                                       end = end.addDays(calendarEvent->getNumRepetition());
                                                       break;
                                                   }
                                                   case CalendarObject::RepetitionType::WEEKLY: {
                                                       start = start.addDays(7 * calendarEvent->getNumRepetition());
                                                       end = end.addDays(7 * calendarEvent->getNumRepetition());
                                                       break;
                                                   }
                                                   case CalendarObject::RepetitionType::MONTHLY: {
                                                       start = start.addMonths(calendarEvent->getNumRepetition());
                                                       end = end.addMonths(calendarEvent->getNumRepetition());
                                                       break;
                                                   }
                                                   case CalendarObject::RepetitionType::YEARLY: {
                                                       start = start.addYears(calendarEvent->getNumRepetition());
                                                       end = end.addYears(calendarEvent->getNumRepetition());
                                                       break;
                                                   }
                                               }
                                               if (!calendarEvent->getExDates().contains(start.date()) &&
                                                   start.date() <= date &&
                                                   end.date() >= date &&
                                                   start.date() <= calendarEvent->getUntilDateRepetition()) {
                                                   return true;
                                               }
                                           }
                                       }
                                   } else {
                                       std::shared_ptr<CalendarToDo> calendarToDo = std::dynamic_pointer_cast<CalendarToDo>(
                                               calendarObject);
                                       QDateTime start;
                                       start = calendarToDo->getStartDateTime();
                                       if (start.date() <= date) {
                                           if (start.date() == date) {
                                               return true;
                                           } else if (calendarToDo->getTypeRepetition() !=
                                                      CalendarObject::RepetitionType::NONE &&
                                                      calendarToDo->getNumRepetition() > 0) {
                                               if (calendarToDo->getUntilDateRepetition() >= date) {
                                                   while (start.date() < date &&
                                                          start.date() <= calendarToDo->getUntilDateRepetition()) {

                                                       switch (calendarToDo->getTypeRepetition()) {
                                                           case CalendarObject::RepetitionType::DAILY: {   // daily
                                                               start = start.addDays(calendarToDo->getNumRepetition());
                                                               break;
                                                           }
                                                           case CalendarObject::RepetitionType::WEEKLY: {      //weekly
                                                               start = start.addDays(
                                                                       7 * calendarToDo->getNumRepetition());
                                                               break;
                                                           }
                                                           case CalendarObject::RepetitionType::MONTHLY: {      //monthly
                                                               start = start.addMonths(
                                                                       calendarToDo->getNumRepetition());
                                                               break;
                                                           }
                                                           case CalendarObject::RepetitionType::YEARLY: {      //yearly
                                                               start = start.addYears(calendarToDo->getNumRepetition());
                                                               break;
                                                           }
                                                       }
                                                       if (start.date() == date &&
                                                           start.date() <= calendarToDo->getUntilDateRepetition()) {
                                                           return true;
                                                       }
                                                   }
                                               }
                                           }

                                       }
                                   }
                                   return false;
                               });
    /*
    for (const auto & calendarObject : calendarObjects) {
        //std::shared_ptr<CalendarObject> calendarObject = calendarObjects[i];
        if (calendarObject->getExDates().contains(date)) {
            continue;
        }
        std::shared_ptr<CalendarEvent> calendarEvent = std::dynamic_pointer_cast<CalendarEvent>(calendarObject);
        if (calendarEvent) {
            if (calendarEvent->getStartDateTime().date() <= date &&
                calendarEvent->getEndDateTime().date() >= date) {
                paintDate(painter, rect);
                return;
            } else if (calendarEvent->getTypeRepetition() != CalendarObject::RepetitionType::NONE &&
                       calendarEvent->getNumRepetition() > 0) {
                QDateTime start = calendarEvent->getStartDateTime();
                QDateTime end = calendarEvent->getEndDateTime();
                while (start.date() < date &&
                       start.date() <= calendarEvent->getUntilDateRepetition()) {
                    // if the event takes more than one day I have to readjust the dates
                    if (start.date() != end.date()) {
                        qint64 diff = start.daysTo(end);
                        start = start.addDays(diff);
                        end = end.addDays(diff);
                    }
                    switch (calendarEvent->getTypeRepetition()) {
                        case CalendarObject::RepetitionType::DAILY: {
                            start = start.addDays(calendarEvent->getNumRepetition());
                            end = end.addDays(calendarEvent->getNumRepetition());
                            break;
                        }
                        case CalendarObject::RepetitionType::WEEKLY: {
                            start = start.addDays(7 * calendarEvent->getNumRepetition());
                            end = end.addDays(7 * calendarEvent->getNumRepetition());
                            break;
                        }
                        case CalendarObject::RepetitionType::MONTHLY: {
                            start = start.addMonths(calendarEvent->getNumRepetition());
                            end = end.addMonths(calendarEvent->getNumRepetition());
                            break;
                        }
                        case CalendarObject::RepetitionType::YEARLY: {
                            start = start.addYears(calendarEvent->getNumRepetition());
                            end = end.addYears(calendarEvent->getNumRepetition());
                            break;
                        }
                    }
                    if (!calendarEvent->getExDates().contains(start.date()) && start.date() <= date &&
                        end.date() >= date &&
                        start.date() <= calendarEvent->getUntilDateRepetition()) {
                        paintDate(painter, rect);
                        return;
                    }
                }
            }
        } else {
            std::shared_ptr<CalendarToDo> calendarToDo = std::dynamic_pointer_cast<CalendarToDo>(calendarObject);
            QDateTime start;
            start = calendarToDo->getStartDateTime();
            if (start.date() <= date) {
                if (start.date() == date) {
                    paintDate(painter, rect);
                    return;
                } else if (calendarToDo->getTypeRepetition() != CalendarObject::RepetitionType::NONE && calendarToDo->getNumRepetition() > 0) {
                    if (calendarToDo->getUntilDateRepetition() >= date) {
                        while (start.date() < date &&
                               start.date() <= calendarToDo->getUntilDateRepetition()) {

                            switch (calendarToDo->getTypeRepetition()) {
                                case CalendarObject::RepetitionType::DAILY: {   // daily
                                    start = start.addDays(calendarToDo->getNumRepetition());
                                    break;
                                }
                                case CalendarObject::RepetitionType::WEEKLY: {      //weekly
                                    start = start.addDays(7 * calendarToDo->getNumRepetition());
                                    break;
                                }
                                case CalendarObject::RepetitionType::MONTHLY: {      //monthly
                                    start = start.addMonths(calendarToDo->getNumRepetition());
                                    break;
                                }
                                case CalendarObject::RepetitionType::YEARLY: {      //yearly
                                    start = start.addYears(calendarToDo->getNumRepetition());
                                    break;
                                }
                            }
                            if (start.date() == date &&
                                start.date() <= calendarToDo->getUntilDateRepetition()) {
                                paintDate(painter, rect);
                                return;
                            }
                        }
                    }
                }

            }
        }

    }
     */

    if (result != calendarObjects.constEnd()) {
        paintDate(painter, rect);
    }
}

void CustomCalendarWidget::paintDate(QPainter *painter, const QRect &rect) {
    QPen pen;
    pen.setColor(QColorConstants::Red);
    painter->setPen(pen);
    painter->setOpacity(0.1);
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColorConstants::Red);
    painter->setBrush(brush);
    painter->drawRect(rect.adjusted(0, 0, -1, -1));
    painter->setOpacity(1);
}

void CustomCalendarWidget::setCalendarObjects(QList<std::shared_ptr<CalendarObject>> calendarObjects) {
    this->calendarObjects.clear();
    this->calendarObjects = std::move(calendarObjects);
    updateCells();
}
