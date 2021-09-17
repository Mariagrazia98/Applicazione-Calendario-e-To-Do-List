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

#include "taskForm.h"
#include "calendarobject.h"
#include "calendarevent.h"
#include "calendarobjectwidget.h"

namespace Ui {
    class Calendar;
}

class Calendar : public QWidget {
Q_OBJECT

public:
    explicit Calendar(QWidget *parent = nullptr);

    ~Calendar();

public slots:
    void parseCalendar(QString calendar);

private slots:

    void selectedDateChanged();

    void reformatCalendarPage();

    void onDateTextChanged();

    void addTaskButtonClicked();

    void parseEvent();

    void onTaskFormClosed();

private:
    Ui::Calendar *ui;
private:
    void setupCalendar();

    void setupWeek();

    void createCalendarGroupBox();

    QComboBox *createColorComboBox();

    QDateTime getDateTimeFromString(const QString& string);

    QGroupBox *calendarGroupBox; // calendar group box (left)
    QGridLayout *calendarLayout;
    QCalendarWidget *calendar;

    QGroupBox *tasksGroupBox; // tasks group box (right)
    QVBoxLayout *tasksLayout;

    QDateEdit *currentDateEdit;
    QTextBrowser *dateString;

    QTextBrowser *answerString;

    QPushButton* addTaskButton;
    //QPushButton* modifyTaskButton;
    //QPushButton* removeTaskButton;

    QTextStream* stream;
    QList<CalendarObject*> calendarObjects;

    QWidget* taskScrollWidget;
    QScrollArea* scrollArea;
    QVBoxLayout* taskViewLayout;
};

#endif // CALENDAR_H
