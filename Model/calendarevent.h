//
// Created by Manuel on 15/09/2021.
//

#ifndef APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDAREVENT_H
#define APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDAREVENT_H

#include "calendarobject.h"

class CalendarEvent : public CalendarObject {
Q_OBJECT
public:
    CalendarEvent();

    CalendarEvent(const CalendarEvent &other);

    ~CalendarEvent() override;

    const QDateTime &getEndDateTime() const;

    void setEndDateTime(const QDateTime &endDateTime);

private:
    QDateTime endDateTime;
};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDAREVENT_H
