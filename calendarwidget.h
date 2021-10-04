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

#include <iostream>

#include "taskForm.h"
#include "calendarobject.h"
#include "calendarevent.h"
#include "calendartodo.h"
#include "calendarobjectwidget.h"
#include "connectionManager.h"

namespace Ui {
    class CalendarWidget;
}

class CalendarWidget : public QWidget {
Q_OBJECT

public:
    explicit CalendarWidget(QWidget *parent = nullptr, ConnectionManager *connectionManager = nullptr);

    ~CalendarWidget();

    void setupConnection();

    void setConnectionManager(ConnectionManager *connectionManager);

    void getCalendarRequest();

    void setupTimer();

public slots:

    void parseCalendar(QString calendar);

    void finished(QNetworkReply *reply);

private slots:

    void selectedDateChanged();

    void reformatCalendarPage();

    void onDateTextChanged();

    void addTaskButtonClicked();

    void parseEvent();

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

    Ui::CalendarWidget *ui;

    QGroupBox *calendarGroupBox; // Calendar group box (left)
    QGridLayout *calendarLayout;
    QCalendarWidget *calendar;

    QGroupBox *tasksGroupBox; // tasks group box (right)
    QVBoxLayout *tasksLayout;

    QDateEdit *currentDateEdit;
    QTextBrowser *dateString;

    QPushButton *addTaskButton;

    QTextStream *stream;
    QList<CalendarObject *> calendarObjects;

    QWidget *taskScrollWidget;
    QScrollArea *scrollArea;
    QVBoxLayout *taskViewLayout;

    ConnectionManager *connectionManager;
    QMetaObject::Connection connectionToFinished;
    QMetaObject::Connection connectionCtag;
    QMetaObject::Connection connectionToModify;
    QMetaObject::Connection connectionToTaskDeleted;

    void addCalendarObjectWidget(CalendarObject *calendarObject);


};

#endif // CALENDAR_H