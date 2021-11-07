//
// Created by Manuel on 15/09/2021.
//

#include <iostream>
#include "calendarobject.h"

CalendarObject::CalendarObject() {

}

CalendarObject::CalendarObject(std::shared_ptr<CalendarObject> other) :
        name(other->name),
        calendarName(other->calendarName),
        UID(other->UID),
        location(other->location),
        description(other->description),
        creationDateTime(other->creationDateTime),
        typeRepetition(other->typeRepetition),
        numRepetition(other->numRepetition),
        priority(other->priority),
        startDateTime(other->startDateTime),
        exDates(other->exDates),
        untilDateRipetition(other->untilDateRipetition) {
    parent = other;
}

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

void CalendarObject::setLocation(const QString location) {
    this->location = location;
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


int CalendarObject::getNumRepetition() const {
    return numRepetition;
}

void CalendarObject::setNumRepetition(int numRepetition) {
    CalendarObject::numRepetition = numRepetition;
}

int CalendarObject::getTypeRepetition() const {
    return typeRepetition;
}

void CalendarObject::setTypeRepetition(int typeRepetition) {
    CalendarObject::typeRepetition = typeRepetition;
}

unsigned int CalendarObject::getPriority() const {
    return priority;
}

void CalendarObject::setPriority(unsigned int priority) {
    if (priority < 0) {
        this->priority = 0;
    } else if (priority > 9) {
        this->priority = 9;
    } else {
        this->priority = priority;
    }
}

const QDate &CalendarObject::getUntilDateRipetition() const {
    return untilDateRipetition;
}

void CalendarObject::setUntilDateRipetition(const QDate &untilDateRipetition) {

    if (untilDateRipetition < startDateTime.date()) {
        this->untilDateRipetition = startDateTime.date();
    } else {
        this->untilDateRipetition = untilDateRipetition;
    }
}

const QDateTime &CalendarObject::getStartDateTime() const {
    return startDateTime;
}

void CalendarObject::setStartDateTime(const QDateTime &startDateTime) {
    this->startDateTime = startDateTime;
}

const QSet<QDate> & CalendarObject::getExDates() const {
    return exDates;
}

void CalendarObject::addExDate(QDate exDate) {
    exDates.insert(exDate);
}

std::weak_ptr<CalendarObject> CalendarObject::getParent() const {
    return parent;
}
