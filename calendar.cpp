//
// Created by manue on 04/10/2021.
//

#include "calendar.h"

Calendar::Calendar(const QString &href, const QString &name, const int ctag)
        : href(href), name(name), ctag(ctag) {

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
