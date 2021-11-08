//
// Created by Manuel on 15/09/2021.
//

#ifndef APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDAROBJECT_H
#define APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDAROBJECT_H

#include <QObject>
#include <QDateTime>
#include <QString>


/**
 *
 * @details CalendarObjects are 'iCalendar' formatted files. They can hold events and to-do's
 * Generally they only hold one event each, but in the case of a recurring event with exceptions, they can hold multiple with the parent attribute.
 */

class CalendarObject : public QObject {
Q_OBJECT
public:

    /**
    * Defines the repetition type for CalendarObjects occurrences
   */
    enum RepetitionType {
        NONE,
        DAILY,
        WEEKLY,
        MONTHLY,
        YEARLY
    };

    /* Constructors */

    /**
     * Default constructor
     */
    CalendarObject();

    /**
     * Custom copy constructor
     *
     * @param other
     *
     * @brief copy constructor and set other as parent of this
     */
    CalendarObject(std::shared_ptr<CalendarObject> other);

    /* Destructor */

    /**
     * Default destructor
     */
    virtual ~CalendarObject() = 0;

    /* Getters */

    /**
     *
     * @return the name of the calendar this CalendarObject relates
     */
    const QString &getCalendarName() const;

    /**
     *
     * @return the name of this CalendarObject
     */
    const QString &getName() const;

    /**
     *
     * @return the location of this CalendarObject
     */
    const QString &getLocation() const;

    /**
     *
     * @return the description of this CalendarObject
     */
    const QString &getDescription() const;

    /**
     *
     * @return the UID of this CalendarObject
     *
     * @brief returns the UID of this CalendarObject, which is the same of his father if this is a recurrence
     */
    const QString &getUID() const;

    /**
     *
     * @return the creationDateTime of this CalendarObject
     */
    const QDateTime &getCreationDateTime() const;

    /**
     *
     * @return the startDateTime of this CalendarObject
     */
    const QDateTime &getStartDateTime() const;

    /**
     *
     * @return the untilDateRepetition of this CalendarObject
     */
    const QDate &getUntilDateRepetition() const;

    /**
     *
     * @return the repetition type of this CalendarObject
     */
    RepetitionType getTypeRepetition() const;

    /**
     *
     * @return the numbers of repetitions this CalendarObject has
     */
    int getNumRepetition() const;

    /**
     *
     * @return the priority of this CalendarObject
     */
    unsigned int getPriority() const;

    /**
     * @brief return the original CalendarObject this CalendarObject refers to if this is a recurrence
     * @return the original CalendarObject this CalendarObject refers to
     */
    std::weak_ptr<CalendarObject> getParent() const;

    /**
     *
     * @return the list of the dates to exclude from this CalendarObject recurrences
     */
    const QSet<QDate> &getExDates() const;

    /* Setters */

    /**
     *
     * @param calendarName the new name of the calendar this CalendarObject refers to
     */
    void setCalendarName(const QString &calendarName);

    /**
     *
     * @param name the new name of this CalendarObject
     */
    void setName(const QString &name);

    /**
     *
     * @param location the new location of this CalendarObject
     */
    void setLocation(const QString location);

    /**
     *
     * @param description the new description of this CalendarObject
     */
    void setDescription(const QString &description);

    /**
     *
     * @param uid the new UID of this CalendarObject
     */
    void setUID(const QString &uid);

    /**
     *
     * @param creationDateTime the new creationDateTime of this CalendarObject
     */
    void setCreationDateTime(const QDateTime &creationDateTime);

    /**
     *
     * @param startDateTime the new startDateTime of this CalendarObject
     */
    void setStartDateTime(const QDateTime &startDateTime);

    /**
     *
     * @param untilDateRipetition the new untilDateTime of this CalendarObject
     */
    void setUntilDateRipetition(const QDate &untilDateRipetition);

    /**
     *
     * @param typeRepetition the new repetition type of this CalendarObject
     */
    void setTypeRepetition(RepetitionType typeRepetition);

    /**
     *
     * @param numRepetition the new number of repetitions of this CalendarObject recurrences
     */
    void setNumRepetition(int numRepetition);

    /**
     * @brief sets the new priority of this CalendarObject clamping from 0 to 9
     * @param priority the new priority of this CalendarObject
     */
    void setPriority(unsigned int priority);

    /* Other */

    /**
     * add a new Date in exDates
     * @param exDate the date to add to exDates
     */
    void addExDate(QDate exDate);

private:
    QString calendarName; // the of the calendar this CalendarObject refers
    QString name; // CalendarObject name
    QString location; // CalendarObject location
    QString description; // CalendarObject description
    QString UID; // the UID of this CalendarObject, which is the same of his father if this is a recurrence
    int numRepetition; // number of future repetitions
    RepetitionType typeRepetition; // describes the type of repetitions of this CalendarObject
    QDateTime creationDateTime; // when this CalendarObject is created
    unsigned int priority; // describes the CalendarObject priority [0-9]
    QDateTime startDateTime; // when this CalendarObject begins
    QDate untilDateRipetition; // last valid recurrence date. If the object has no recurrences, it coincides to startDateTime.
    QSet<QDate> exDates; // dates excluded by recurrence
    std::weak_ptr<CalendarObject> parent; // this CalendarObject parent, it's null if this is a recurrence

};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDAROBJECT_H
