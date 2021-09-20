//
// Created by Manuel on 20/09/2021.
//

#include "calendartodo.h"

unsigned int CalendarToDo::getPriority() const {
    return priority;
}

void CalendarToDo::setPriority(unsigned int priority) {
    if (priority < 0) {
        CalendarToDo::priority = 0;
    } else if (priority > 9) {
        CalendarToDo::priority = 9;
    } else {
        CalendarToDo::priority = priority;
    }
}

const std::optional<QDateTime> &CalendarToDo::getStartDateTime() const {
    return startDateTime;
}

void CalendarToDo::setStartDateTime(const std::optional<QDateTime> &startDateTime) {
    if (dueDateTime && startDateTime && dueDateTime < startDateTime) {
        CalendarToDo::startDateTime = dueDateTime;
    } else {
        CalendarToDo::startDateTime = startDateTime;
    }

}

const std::optional<QDateTime> &CalendarToDo::getDueDateTime() const {
    return dueDateTime;
}

void CalendarToDo::setDueDateTime(const std::optional<QDateTime> &dueDateTime) {
    if (dueDateTime && startDateTime && dueDateTime < startDateTime) {
        CalendarToDo::dueDateTime = startDateTime;
    } else {
        CalendarToDo::dueDateTime = dueDateTime;
    }

}

const std::optional<QDateTime> &CalendarToDo::getCompletedDateTime() const {
    return completedDateTime;
}

void CalendarToDo::setCompletedDateTime(const QDateTime &completedDateTime) {
    CalendarToDo::completedDateTime = completedDateTime;
}

ToDoStatus CalendarToDo::getStatus() const {
    return status;
}

void CalendarToDo::setStatus(ToDoStatus status) {
    CalendarToDo::status = status;
}
