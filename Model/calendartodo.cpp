//
// Created by Manuel on 20/09/2021.
//

#include <iostream>
#include "calendartodo.h"


CalendarToDo::CalendarToDo() {}


CalendarToDo::CalendarToDo(std::shared_ptr<CalendarToDo> other) :
        CalendarObject(other) {
    completedDateList = other->completedDateList;
}

QList<QDate> &CalendarToDo::getCompletedDate() {
    return completedDateList;
}

void CalendarToDo::addCompletedDate(const QDate &completedDate) {
    this->completedDateList.append(completedDate);
}

void CalendarToDo::removeCompletedDate(const QDate &completedDate) {
    completedDateList.removeAll(completedDate);
}
