//
// Created by Manuel on 20/09/2021.
//

#ifndef APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDARTODO_H
#define APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDARTODO_H

#include "calendarobject.h"

#include <QObject>
#include <optional>

enum class ToDoStatus {
    COMPLETED,
    NEEDS_ACTION,
    IN_PROCESS,
    CANCELLED
};

class CalendarToDo : public CalendarObject {
Q_OBJECT
public:
    CalendarToDo();

    CalendarToDo(const CalendarToDo &other);

    std::optional<QDateTime> &getCompletedDateTime();

    void setCompletedDateTime(const std::optional<QDateTime> completedDateTime);

    ToDoStatus getStatus() const;

    void setStatus(ToDoStatus status);


private:
    std::optional<QDateTime> completedDateTime;
    ToDoStatus status;
};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDARTODO_H
