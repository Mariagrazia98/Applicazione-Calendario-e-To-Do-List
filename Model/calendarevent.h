//
// Created by Manuel on 15/09/2021.
//

#ifndef APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDAREVENT_H
#define APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDAREVENT_H

#include "calendarobject.h"

/**
 *
 * @details CalendarEvent is one of the possible types of CalendarObjects. A VEVENT in 'iCalendar'  describes an event,
 * which has a scheduled amount of time on a calendar. Compared to CalendarTodos, CalendarEvents are
 * characterized by a DateTime value which represents the moment in which and event ends.
 */

class CalendarEvent : public CalendarObject {
Q_OBJECT
public:

    /* Constructors */

    /**
     * Default constructor
     */
    CalendarEvent();

    /**
    * Custom copy constructor
    *
    * @param other
    *
    * @brief copy constructor and set other as parent of this
    */
    CalendarEvent(std::shared_ptr<CalendarEvent> other);

    /**
    *
    * @return the endDateTime of this CalendarEvent
    */
    const QDateTime &getEndDateTime() const;

    /**
    *
    * @param endDateTime the new endDateTime of this CalendarEvent
    */
    void setEndDateTime(const QDateTime &endDateTime);

private:
    QDateTime endDateTime;  // when this CalendarEvent ends
};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDAREVENT_H