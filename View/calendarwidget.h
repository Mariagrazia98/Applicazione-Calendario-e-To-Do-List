#ifndef CALENDAR_H
#define CALENDAR_H

#include <QWidget>
#include <QDateTime>
#include <QTextBrowser>
#include <QCalendarWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLocale>
#include <QTextCharFormat>
#include <QMessageBox>
#include <QTextBrowser>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QPushButton>
#include <QDebug>
#include <QScrollArea>
#include <QTimer>
#include <QMainWindow>
#include <QDockWidget>
#include <QtConcurrent>

#include <iostream>
#include <istream>
#include <execution>

#include "calendarobjectform.h"
#include "../Model/calendarobject.h"
#include "../Model/calendarevent.h"
#include "../Model/calendartodo.h"
#include "calendarobjectwidget.h"
#include "../Controller/connectionmanager.h"
#include "../Controller/utils.h"
#include "customcalendarwidget.h"
#include "sharecalendarform.h"

class CalendarWidget : public QMainWindow {
Q_OBJECT

public:
    /**
    * Constructor
    * @brief set up widget
    */
    explicit CalendarWidget(QWidget *parent = nullptr);

    /**
    * @brief setup connection manager
    * Each connection manager makes a get for a calendar
    */
    void setupConnection();

    /**
     * @brief add an existing connection manager to calendar widget
     * @param connectionManager to be added
     */
    void addConnectionManager(ConnectionManager *connectionManager);

    /**
     * @brief get the calendar
     * @details When the timer expires, the connection manager, associated to a certain calendar, makes the request
     * @param Name of the calendar to get
     */
    void getCalendarRequest(const QString& calendarName);

    /**
    * @brief setups and starts the timer
    */
    void setupTimer();


public slots:


    /***
     * @brief calls the function parseCalendar in order to parse the calendar and starts timer
     * @details This is called when the signal "calendarReady" connected to the slot is emitted by the ConnectionManager object .
     * First of all, the function verifies if everything went well.
     * If there were not error, it calls the parseCalendar function and starts timer
     * Otherwise, a QMessageBox will appear
     */
    void onCalendarReady(QNetworkReply *reply);

private slots:

    /***
    * @brief update the variable currentDateEdit that stores the current date selected and dateString that visualizes the current date selected
    * @details This is called when the signal "selectionChanged" connected to the slot is emitted by QCalendarWidget object.
    * The signal is emitted when the currently selected date of the calendar changes.
    */
    void selectedDateChanged();

    /***
    * @brief shows the CalendarObjectForm
    * @details This is called when the signal "QPushButton::clicked" connected to the slot is emitted by addCalendarObjectButton.
    */
    void addCalendarObjectButtonClicked();

    /***
    * @brief shows the ShareCalendarForm
    * @details This is called when the signal "QPushButton::clicked" connected to the slot is emitted by shareCalendarButton.
    */
    void shareCalendarButtonClicked();


    /***
     * @brief enables shareCalendarButton
     * @details This is called when the signal "shareCalendarFormClosed" connected to the slot is emitted by shareCalendarForm.
     */
    void onShareCalendarFormClosed();

    /***
     * @brief enables addCalendarObjectButton
     * @details This is called when the signal "taskFormClosed" connected to the slot is emitted by taskForm.
     */
    void onTaskFormClosed();

    /***
     * @brief stops the timer calls the function getctag
     * @details This is called when the signal "taskUploaded" connected to the slot is emitted by taskForm.
     */
    void onTaskModified(const QString &calendarName);

    /***
     * @brief stops the timer and calls the function getctag
     * @details This is called when the signal "taskDeleted" connected to the slot is emitted by CalendarObjectWidget.
     */
    void onTaskDeleted(const CalendarObject &obj);

    /***
    * @brief each connection manager calls the function getctag
    * @details This is called when the signal "timeout" connected to the slot is emitted by the timer.
    * The signal is emitted when the timer times out.
    */
    void onTimeout();

private:

    /***
     * @brief  setup the visualization of the calendar and the button to add a new calendar object
     */
    void setupCalendar();

    /**
      * Getter
      * @brief return the current date selected
      */
    QDate getCurrentDateSelected();

    /***
     * @brief parse the calendar received as a QString.
     * @param QString calendarString: a calendar in a QString format
     * @details It will call the function calendarObject distinguishing an event from a to-do
     * If there were not error, it calls the parseCalendar function and starts timer
     * Otherwise, a QMessageBox will appear
     */
    void parseCalendar(QString calendar);

    //std::shared_ptr<CalendarObject> parseCalendarObject_parallel(const QString &calendarObjectString);


    /**
    * @brief setup CalendarBox and shareCalendarButton
    */
    void createCalendarGroupBox();

    /**
    * @brief shows all the calendar objects in the data selected
    */
    void showSelectedDateCalendarObjects();

    /**
    * @brief add a new calendarObjectWidget to taskViewLayout
    * @param the shared pointer to a calendar object
    * @details create a new CalendarObjectWidget object and adds it to taskViewLayout in order to visualize it
    */
    void addCalendarObjectWidget(const std::shared_ptr<CalendarObject>& calendarObject);


    /* Attributes */
    QGroupBox *calendarGroupBox{}; /**< shows the calendar and button for sharing a calendar on the left side of the window */
    QGridLayout *calendarLayout{};
    CustomCalendarWidget *calendar{}; /**< Custom calendar widget */

    QGroupBox *tasksGroupBox{}; /**< shows the list of events and to-dos in the day selected and a button for adding a new calendar object.
 * They are showed on the right side of the window */
    QVBoxLayout *tasksLayout{};

    QDateEdit *currentDateEdit{}; /**< shows the current date selected on the top-right side of the window */
    QTextBrowser *dateString; /**< shows the current date selected on the top-right side of the window */

    QPushButton *addCalendarObjectButton{}; /**< button for a adding a new calendarObject */
    QPushButton *shareCalendarButton; /**< button for sharing a calendar */

    QTextStream *stream; /** stores the lines read in parseCalendar*/
    QList<std::shared_ptr<CalendarObject>> calendarObjects; /** list of calendar objects*/

    QWidget *taskScrollWidget{};
    QScrollArea *scrollArea{};
    QVBoxLayout *taskViewLayout{};

    QMap<QString, std::shared_ptr<ConnectionManager>> connectionManagers;

    std::unique_ptr<QTimer> timer;  /**< When the timer expires, check whether the ctag of a calendar has changed */
    const unsigned int timerInterval;


};

#endif // CALENDAR_H
