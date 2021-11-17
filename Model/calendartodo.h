//
// Created by Manuel on 20/09/2021.
//

#ifndef APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDARTODO_H
#define APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDARTODO_H

#include "calendarobject.h"
#include <QObject>
#include <optional>

class CalendarToDo : public CalendarObject {
Q_OBJECT
public:

    /**
     * Constructor
     */
    CalendarToDo();

    /* TODO: è un costruttore?*/
    CalendarToDo(std::shared_ptr<CalendarToDo> other);

    /**
     * Getter
     * @return the list of dates where the task has completed
     */
    QList<QDate> &getCompletedDate();

    /**
     * add a new Date in completedDateList .
     * @param the date that will be add.
     */
    void addCompletedDate(const QDate& completedDate);

    /**
    * remove the Date passed as params in completedDateList .
    * @param the date that will be remove.
    */
    void removeCompletedDate(const QDate& completedDate);

private:
    QList<QDate> completedDateList; /*! < describes the dates in which the to-do has completed. */
};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDARTODO_H
