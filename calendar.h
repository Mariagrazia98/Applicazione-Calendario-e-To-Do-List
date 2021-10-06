//
// Created by manue on 04/10/2021.
//

#ifndef APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDAR_H
#define APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDAR_H


#include <QObject>
#include <QString>

struct Calendar : public QObject {
    Q_OBJECT
public:

    Calendar(const QString &href = "", const QString &name = "", const int ctag = -1);

    void setHref(const QString &href);

    void setName(const QString &name);

    void setCtag(int ctag);

    const QString &getHref() const;

    const QString &getName() const;

    int getCtag() const;

private:
    QString href;
    QString name;
    int ctag;

};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDAR_H
