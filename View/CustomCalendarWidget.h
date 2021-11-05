//
// Created by manue on 09/10/2021.
//

#ifndef APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CUSTOMCALENDARWIDGET_H
#define APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CUSTOMCALENDARWIDGET_H

#include <QCalendarWidget>
#include <QPainter>
#include <QList>
#include "../Model/calendarobject.h"

class CustomCalendarWidget : public QCalendarWidget {
Q_OBJECT
public:
    explicit CustomCalendarWidget(QWidget *parent = nullptr);

    void setCalendarObjects( QList<std::shared_ptr<CalendarObject>> calendarObjects);

protected:
    void paintCell(QPainter *painter, const QRect &rect, QDate date) const override;

private:
    QList<std::shared_ptr<CalendarObject>> calendarObjects; // metterlo weak

    void paintDate(QPainter *painter, const QRect &rect) const;
};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CUSTOMCALENDARWIDGET_H
