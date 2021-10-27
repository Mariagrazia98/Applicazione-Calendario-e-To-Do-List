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
    explicit CalendarWidget(QWidget *parent = nullptr, ConnectionManager *connectionManager = nullptr);

    ~CalendarWidget();

    void setupConnection();

    void setConnectionManager(ConnectionManager *connectionManager);

    void getCalendarRequest();

    void setupTimer();

    QDate getCurrentDateSelected();

public
    slots:

            void parseCalendar(QString
    calendar);

    void onCalendarReady(QNetworkReply *reply);

private
    slots:

            void selectedDateChanged();

    void reformatCalendarPage();

    void addTaskButtonClicked();

    void shareCalendarButtonClicked();

    void parseEvent();

    void onSharecalendarFormClosed();

    void onTaskFormClosed();

    void onTaskModified();

    void onTaskDeleted(CalendarObject &obj);

    void onTimeout();


private:
    void setupCalendar();

    void setupWeek();

    void createCalendarGroupBox();

    void showSelectedDateTasks();

    QComboBox *createColorComboBox();

    QDateTime getDateTimeFromString(const QString &string);

    void parseToDo();

    QGroupBox *calendarGroupBox; // Calendar group box (left)
    QGridLayout *calendarLayout;
    CustomCalendarWidget *calendar;

    QGroupBox *tasksGroupBox; // tasks group box (right)
    QVBoxLayout *tasksLayout;

    QDateEdit *currentDateEdit;
    QTextBrowser *dateString;

    QPushButton *addTaskButton;
    QPushButton *shareCalendarButton;


    QTextStream *stream;
    QList<CalendarObject *> calendarObjects;

    QWidget *taskScrollWidget;
    QScrollArea *scrollArea;
    QVBoxLayout *taskViewLayout;

    std::shared_ptr<ConnectionManager *> connectionManager;

    QTimer *timer;
    const unsigned int timerInterval;

    void addCalendarObjectWidget(CalendarObject *calendarObject);

    void addExDatesToCalendarObject(CalendarObject *calendarObject, QString &value);

};

#endif // CALENDAR_H
