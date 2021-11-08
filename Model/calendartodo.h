//
// Created by Manuel on 20/09/2021.
//

#ifndef APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDARTODO_H
#define APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDARTODO_H

#include "calendarobject.h"
#include <QObject>
#include <optional>

/***
 * @brief extends calendar object class.
 * @details add other calendar properties that describe a to-do
 */
enum class ToDoStatus {
    COMPLETED, /* To-do completed */
    NEEDS_ACTION, /* To-do needs action */
    IN_PROCESS, /* To-do in process of being completed */
    CANCELLED /* to-do was cancelled */
};

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
     * Getter
     * @return the status of the to-do component
     */
    ToDoStatus getStatus() const;

    /**
     * Setter
     * @param status
     */
    void setStatus(ToDoStatus status);

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
                                    /*!<  If the to-do has not repetition, it will contain only one element*/
    ToDoStatus status; /*!< define the status of the calendar to-do */
};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDARTODO_H
