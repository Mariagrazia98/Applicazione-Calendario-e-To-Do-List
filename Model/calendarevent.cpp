//
// Created by Manuel on 15/09/2021.
//

#include <iostream>
#include "calendarevent.h"

CalendarEvent::CalendarEvent() {

}

CalendarEvent::CalendarEvent(std::shared_ptr<CalendarEvent> other) :
    CalendarObject(other),
    endDateTime(other->endDateTime) {
}

const QDateTime &CalendarEvent::getEndDateTime() const {
    return endDateTime;
}

void CalendarEvent::setEndDateTime(const QDateTime &endDateTime) {
    CalendarEvent::endDateTime = endDateTime;
}