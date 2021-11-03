//
// Created by Manuel on 20/09/2021.
//

#include <iostream>
#include "calendartodo.h"


CalendarToDo::CalendarToDo() {}

CalendarToDo::CalendarToDo(const CalendarToDo &other) :
        CalendarObject(other),
        status(other.status) {
    if (other.completedDateTime) {
        completedDateTime = *other.completedDateTime;
    } else {
        completedDateTime = std::nullopt;
    }
}


std::optional<QDateTime> &CalendarToDo::getCompletedDateTime() {
    return completedDateTime;
}

void CalendarToDo::setCompletedDateTime(const std::optional<QDateTime> completedDateTime) {
    CalendarToDo::completedDateTime = completedDateTime;
}

ToDoStatus CalendarToDo::getStatus() const {
    return status;
}

void CalendarToDo::setStatus(ToDoStatus status) {
    CalendarToDo::status = status;
}
