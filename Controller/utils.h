//
// Created by manue on 24/11/2021.
//

#ifndef APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_UTILS_H
#define APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_UTILS_H

#include <Qt>
#include <QDateTime>

class Utils {
public:
    /**
     * @brief get a QDateTime from a QString
     * @param a Qstring date
     * @return QDateTime date
     */
    static QDateTime getDateTimeFromString(const QString &string) {
        QDateTime dateTime = QDateTime::fromString(string, "yyyyMMdd'T'hhmmss'Z'");
        if (!dateTime.isValid())
            dateTime = QDateTime::fromString(string, "yyyyMMdd'T'hhmmss");
        if (!dateTime.isValid())
            dateTime = QDateTime::fromString(string, "yyyyMMddhhmmss");
        if (!dateTime.isValid())
            dateTime = QDateTime::fromString(string, "yyyyMMdd");
        if (!dateTime.isValid())
            std::cerr << "CalendarWidget: could not parse " << string.toStdString() << '\n';
        return dateTime;
    }

    /**
    * @brief parses the Qstring value received as param and for each exDate calls the function addExDate of calendarObject
    * @param the pointer to a calendar object and the list of exDates in a QString format
    */
    static QString addExDatesToCalendarObject(CalendarObject *calendarObject, QString &value) {
        int endDeliminatorPosition = value.indexOf(QLatin1Char('Z'));
        QList<QDate> exDates;
        while (endDeliminatorPosition > 0 && !value.isEmpty()) {
            const QString exDateString = value.mid(0, endDeliminatorPosition + 1);
            const QDate exDate = Utils::getDateTimeFromString(exDateString).toLocalTime().date();
            calendarObject->addExDate(exDate);
            value = value.mid(endDeliminatorPosition + 2, -1);
            endDeliminatorPosition = value.indexOf(QLatin1Char('Z'));
        }
        return value;
    }

    /**
     * @brief parses the Qstring value received as param and for each completeDate calls the function addCompleteDate of calendarToDo
     * @param the pointer to a calendar to-do object and the list of completeDate in a QString format
     */
    static QString addCompletedDatesToCalendarObject(CalendarToDo *calendarTodo, QString &value) {
        int endDeliminatorPosition = value.indexOf(QLatin1Char('Z'));
        while (endDeliminatorPosition > 0 && !value.isEmpty()) {
            const QString completedDateString = value.mid(0, endDeliminatorPosition + 1);
            const QDate completedDate = Utils::getDateTimeFromString(completedDateString).toLocalTime().date();
            calendarTodo->addCompletedDate(completedDate);
            value = value.mid(endDeliminatorPosition + 2, -1);
            endDeliminatorPosition = value.indexOf(QLatin1Char('Z'));
        }
        return value;
    }

};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_UTILS_H
