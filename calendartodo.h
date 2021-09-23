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

    unsigned int getPriority() const;

    void setPriority(unsigned int priority);

    const std::optional<QDateTime> &getStartDateTime() const;

    void setStartDateTime(const QDateTime &startDateTime);

    const std::optional<QDateTime> &getDueDateTime() const;

    void setDueDateTime(const QDateTime &dueDateTime);

    std::optional<QDateTime> &getCompletedDateTime();

    void setCompletedDateTime(const QDateTime &completedDateTime);

    ToDoStatus getStatus() const;

    void setStatus(ToDoStatus status);

private:
    std::optional<QDateTime> completedDateTime;
    unsigned int priority; // [0-9]
    ToDoStatus status;

    std::optional<QDateTime> startDateTime;
    std::optional<QDateTime> dueDateTime;

};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDARTODO_H
