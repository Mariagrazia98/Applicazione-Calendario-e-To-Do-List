//
// Created by manue on 04/10/2021.
//

#include "calendar.h"

Calendar::Calendar(const QString &href, const QString &name, const QString &displayName, const int ctag)
        : href(href), name(name), ctag(ctag), displayName(displayName) {

}

Calendar::Calendar(const Calendar &other) :
        href(other.href), name(other.name), ctag(other.ctag) {

}

const QString &Calendar::getHref() const {
    return href;
}

const QString &Calendar::getName() const {
    return name;
}

int Calendar::getCtag() const {
    return ctag;
}

void Calendar::setHref(const QString &href) {
    Calendar::href = href;
}

void Calendar::setName(const QString &name) {
    Calendar::name = name;
}

void Calendar::setCtag(int ctag) {
    Calendar::ctag = ctag;
}

const QString &Calendar::getDisplayName() const {
    return displayName;
}


