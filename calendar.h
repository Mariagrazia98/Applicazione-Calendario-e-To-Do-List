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

namespace Ui {
class Calendar;
}

class Calendar : public QWidget
{
    Q_OBJECT

public:
    explicit Calendar(QWidget *parent = nullptr);
    ~Calendar();

private slots:
    void selectedDateChanged();
    void reformatCalendarPage();

    void responseHandler(QNetworkReply*);
    void authenticationRequired(QNetworkReply *, QAuthenticator *);

private:
    Ui::Calendar *ui;
private:
    void setupCalendar();
    void setupWeek();
    void createPreviewGroupBox();
    void createGeneralOptionsGroupBox();
    QComboBox *createColorComboBox();

    /* Network functions */
    void setupRequest();


    QGroupBox *previewGroupBox;
    QGridLayout *previewLayout;
    QCalendarWidget *calendar;

    QGroupBox *generalOptionsGroupBox;
    QVBoxLayout *tasksLayout;

    QDateEdit *currentDateEdit;
    QTextBrowser *dateString;

    QTextBrowser *answerString;

    QNetworkAccessManager* networkAccessManager;
};

#endif // CALENDAR_H
