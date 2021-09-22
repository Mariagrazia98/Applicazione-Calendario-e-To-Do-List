//
// Created by Manuel on 15/09/2021.
//

#include <iostream>
#include "calendarevent.h"

CalendarEvent::CalendarEvent() {

}

CalendarEvent::CalendarEvent(const CalendarEvent &other) : CalendarObject(other),
                                                           startDateTime(other.startDateTime),
                                                           endDateTime(other.endDateTime) {

}

CalendarEvent::~CalendarEvent() {

}

const QDateTime &CalendarEvent::getStartDateTime() const {
    return startDateTime;
}

void CalendarEvent::setStartDateTime(const QDateTime &startDateTime) {
    CalendarEvent::startDateTime = startDateTime;
}

const QDateTime &CalendarEvent::getEndDateTime() const {
    return endDateTime;
}

void CalendarEvent::setEndDateTime(const QDateTime &endDateTime) {
    CalendarEvent::endDateTime = endDateTime;
}

void CalendarEvent::printCalendarObject() {
    CalendarObject::printCalendarObject();
    std::cout << "startDateTime: " << startDateTime.toString("yyyyMMddhhmmss").toStdString() << '\n';
    std::cout << "endDateTime: " << endDateTime.toString("yyyyMMddhhmmss").toStdString() << '\n';
}


