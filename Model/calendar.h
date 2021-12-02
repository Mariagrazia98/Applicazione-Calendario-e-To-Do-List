//
// Created by manue on 04/10/2021.
//

#ifndef APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDAR_H
#define APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDAR_H


#include <QObject>
#include <QString>

/***
 * @brief used to store general information about the calendar.
 */

class Calendar : public QObject {
Q_OBJECT
public:

    /**
     * Constructor
     * @param href
     * @param name
     * @param ctag
     */
    Calendar(const QString &href = "", const QString &name = "", const QString &displayName = "", const int ctag = -1);

    /**
     * Copy constructor
     * @param other calendar
     */
    Calendar(const Calendar &other);

    /**
     * Setter
     * @param href
     */
    void setHref(const QString &href);

    /**
     * Setter
     * @param name
     */
    void setName(const QString &name);

    /**
     * Setter
     * @param ctag
     */
    void setCtag(int ctag);

    /**
     * Getter
     * @return calendar href
     */
    const QString &getHref() const;

    /**
     * Getter
     * @return calendar name
     */
    const QString &getName() const;

    /**
     * Getter
     * @return calendar ctag
     */
    int getCtag() const;

    /**
      * Getter
      * @return calendar display name
      */
    const QString &getDisplayName() const;


private:
    QString name; /**< name of the calendar */
    QString displayName; /**< display name of the calendar */
    QString href; /**< indicates the URL */
    int ctag; /**< ctag works like a change id. Every time the ctag has changed, something in the calendar has changed too. */
};

#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDAR_H
