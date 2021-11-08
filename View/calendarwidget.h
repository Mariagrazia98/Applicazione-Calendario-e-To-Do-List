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

#include <iostream>

#include "taskform.h"
#include "../Model/calendarobject.h"
#include "../Model/calendarevent.h"
#include "../Model/calendartodo.h"
#include "calendarobjectwidget.h"
#include "../Controller/connectionManager.h"
#include "CustomCalendarWidget.h"
#include "sharecalendarform.h"

class CalendarWidget : public QMainWindow {
Q_OBJECT

public:
    explicit CalendarWidget(QWidget *parent = nullptr);

    ~CalendarWidget();

    void setupConnection();

    void addConnectionManager(ConnectionManager *connectionManager);

    void getCalendarRequest(const QString calendarName);

    void setupTimer();

    QDate getCurrentDateSelected();

public
    slots:

    void parseCalendar(QString calendar);

    void onCalendarReady(QNetworkReply *reply);

private
    slots:

    void selectedDateChanged();

    void reformatCalendarPage();

    void addCalendarObjectButtonClicked();

    void shareCalendarButtonClicked();

    void parseEvent(const QString &calendarName);

    void onSharecalendarFormClosed();

    void onTaskFormClosed();

    void onTaskModified(const QString calendarName);

    void onTaskDeleted(CalendarObject &obj);

    void onTimeout();


private:
    void setupCalendar();

    void setupWeek();

    void createCalendarGroupBox();

    void showSelectedDateTasks();

    QComboBox *createColorComboBox();

    QDateTime getDateTimeFromString(const QString &string);

    void parseToDo(const QString &calendarName);

    void addCalendarObjectWidget(std::shared_ptr<CalendarObject> calendarObject);
    QString addExDatesToCalendarObject(CalendarObject *calendarObject, QString &value);


    /* Attributes */
    QGroupBox *calendarGroupBox; /**< shows the calendar and button for sharing a calendar on the left side of the window */
    QGridLayout *calendarLayout;
    CustomCalendarWidget *calendar; /**< Custom calendar widget */

    QGroupBox *tasksGroupBox; /**< shows the list of events and to-dos in the day selected and a button for adding a new calendar object.
 * They are showed on the right side of the window */
    QVBoxLayout *tasksLayout;

    QDateEdit *currentDateEdit;
    QTextBrowser *dateString; /**< shows the current date selected on the top-right side of the window */

    QPushButton *addCalendarObjectButton; /**< button for a adding a new calendarObject */
    QPushButton *shareCalendarButton; /**< button for sharing a calendar */

    QTextStream *stream;
    QList<std::shared_ptr<CalendarObject>> calendarObjects;
    QString addCompletedDatesToCalendarObject(CalendarToDo* calendarTodo, QString &value);

    QWidget *taskScrollWidget;
    QScrollArea *scrollArea;
    QVBoxLayout *taskViewLayout;

    QMap<QString, std::shared_ptr<ConnectionManager>> connectionManagers;

    QTimer *timer;
    const unsigned int timerInterval;


};

#endif // CALENDAR_H
