//
// Created by Manuel on 15/09/2021.
//

#ifndef APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDAROBJECT_H
#define APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDAROBJECT_H

#include <QObject>
#include <QDateTime>
#include <QString>

class CalendarObject : public QObject {
Q_OBJECT
public:

    virtual ~CalendarObject()=0;

    virtual void printCalendarObject();

    const QString &getCalendarName() const;

    void setCalendarName(const QString &calendarName);

    const QString &getName() const;

    void setName(const QString &name);

    const QString &getLocation() const;

    void setLocation(const QString &location);

    const QString &getDescription() const;

    void setDescription(const QString &description);

    const QString &getUID() const;

    void setUID(const QString &uid);

    const QDateTime &getCreationDateTime() const;

    void setCreationDateTime(const QDateTime &creationDateTime);

private:

    QString calendarName;
    QString name;
    QString location;
    QString description;
    QString UID;
    int numRepetition;
    QString typeRepetition;
    QDateTime creationDateTime;
public:
    int getNumRepetition() const;

    void setNumRepetition(int numRepetition);

    const QString &getTypeRepetition() const;

    void setTypeRepetition(const QString &typeRepetition);

};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDAROBJECT_H
