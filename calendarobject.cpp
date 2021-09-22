//
// Created by Manuel on 15/09/2021.
//

#include <iostream>
#include "calendarobject.h"

CalendarObject::~CalendarObject() {

}

const QString &CalendarObject::getCalendarName() const {
    return calendarName;
}

void CalendarObject::setCalendarName(const QString &calendarName) {
    CalendarObject::calendarName = calendarName;
}

const QString &CalendarObject::getName() const {
    return name;
}

void CalendarObject::setName(const QString &name) {
    CalendarObject::name = name;
}

const QString &CalendarObject::getLocation() const {
    return location;
}

void CalendarObject::setLocation(const QString &location) {
    CalendarObject::location = location;
}

const QString &CalendarObject::getDescription() const {
    return description;
}

void CalendarObject::setDescription(const QString &description) {
    CalendarObject::description = description;
}

const QString &CalendarObject::getUID() const {
    return UID;
}

void CalendarObject::setUID(const QString &uid) {
    CalendarObject::UID = uid;
}

const QDateTime &CalendarObject::getCreationDateTime() const {
    return creationDateTime;
}

void CalendarObject::setCreationDateTime(const QDateTime &creationDateTime) {
    CalendarObject::creationDateTime = creationDateTime;
}

void CalendarObject::printCalendarObject() {
    std::cout << "Name: " << name.toStdString() << '\n';
    std::cout << "Description: " << description.toStdString() << '\n';
    std::cout << "UID: " << UID.toStdString() << '\n';
}

int CalendarObject::getNumRepetition() const {
    return numRepetition;
}

void CalendarObject::setNumRepetition(int numRepetition) {
    CalendarObject::numRepetition = numRepetition;
    std::cout<<numRepetition<<std::endl;
}

int CalendarObject::getTypeRepetition() const {
    return typeRepetition;
}

void CalendarObject::setTypeRepetition(int typeRepetition) {
    CalendarObject::typeRepetition = typeRepetition;
}




