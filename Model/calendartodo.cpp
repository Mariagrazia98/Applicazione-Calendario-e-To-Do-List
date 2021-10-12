//
// Created by Manuel on 20/09/2021.
//

#include <iostream>
#include "calendartodo.h"


CalendarToDo::CalendarToDo() {
    std::cout<< "todo construct\n";
    if(startDateTime){
        std::cout<< "todo construct"<<startDateTime->toString().toStdString();
    }

}

CalendarToDo::CalendarToDo(const CalendarToDo &other) :
        CalendarObject(other),
        status(other.status) {
    std::cout << " Copy constructor\n";
    if (other.completedDateTime) {
        completedDateTime = *other.completedDateTime;
        std::cout<<"nullopt"<< completedDateTime->toString().toStdString();
    } else {
        completedDateTime = std::nullopt;
        std::cout << " Copy constructor - else 1\n";
    }
    if (other.startDateTime) {
        startDateTime = *other.startDateTime;
        std::cout<< "start IF-2"<<startDateTime->toString().toStdString();
    } else {
        startDateTime = std::nullopt;
        std::cout << " Copy constructor else 2\n";
    }
    if (other.dueDateTime) {
        std::cout << " Copy constructor IF 3\n";
        dueDateTime = *other.dueDateTime;
        std::cout<<"due date time"<< dueDateTime->toString().toStdString();

    } else {
        dueDateTime = std::nullopt;
        std::cout << " Copy constructor else 3\n";
    }
}


const std::optional<QDateTime> &CalendarToDo::getStartDateTime() const {
    return startDateTime;
}

void CalendarToDo::setStartDateTime(const QDateTime &startDateTime) {
    if(!startDateTime.isNull()){
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

void CalendarToDo::setCompletedDateTime(const std::optional<QDateTime> &completedDateTime) {
    CalendarToDo::completedDateTime = completedDateTime;
}

ToDoStatus CalendarToDo::getStatus() const {
    return status;
}

void CalendarToDo::setStatus(ToDoStatus status) {
    CalendarToDo::status = status;
}
