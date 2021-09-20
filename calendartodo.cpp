//
// Created by Manuel on 20/09/2021.
//

#include "calendartodo.h"

unsigned int CalendarToDo::getPriority() const {
    return priority;
}

void CalendarToDo::setPriority(unsigned int priority) {
    if (priority < 0) {
        this->priority = 0;
    } else if (priority > 9) {
        this->priority = 9;
    } else {
        this->priority = priority;
    }
}

const std::optional<QDateTime> &CalendarToDo::getStartDateTime() const {
    return startDateTime;
}

void CalendarToDo::setStartDateTime(const QDateTime &startDateTime) {
    if (dueDateTime && this->startDateTime && dueDateTime < startDateTime) {
        this->startDateTime = dueDateTime;
    } else {
        this->startDateTime = startDateTime;
    }

}

const std::optional<QDateTime> &CalendarToDo::getDueDateTime() const {
    return dueDateTime;
}

void CalendarToDo::setDueDateTime(const QDateTime &dueDateTime) {
    if (this->dueDateTime && startDateTime && dueDateTime < startDateTime) {
        this->dueDateTime = startDateTime;
    } else {
        this->dueDateTime = dueDateTime;
    }

}

std::optional<QDateTime> &CalendarToDo::getCompletedDateTime() {
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
