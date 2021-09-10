#ifndef CALENDAR_H
#define CALENDAR_H

#include <QWidget>
#include <QDateTime>
#include <QTextBrowser>

namespace Ui {
class Calendar;
}

QT_BEGIN_NAMESPACE
class QCalendarWidget;
class QCheckBox;
class QComboBox;
class QDate;
class QDateEdit;
class QGridLayout;
class QGroupBox;
class QLabel;
QT_END_NAMESPACE

class Calendar : public QWidget
{
    Q_OBJECT

public:
    explicit Calendar(QWidget *parent = nullptr);
    ~Calendar();

private slots:
    void selectedDateChanged();
    void reformatCalendarPage();

private:
    Ui::Calendar *ui;
private:
    void setupCalendar();
    void setupWeek();
    void createPreviewGroupBox();
    void createGeneralOptionsGroupBox();
    QComboBox *createColorComboBox();

    QGroupBox *previewGroupBox;
    QGridLayout *previewLayout;
    QCalendarWidget *calendar;

    QGroupBox *generalOptionsGroupBox;

    QDateEdit *currentDateEdit;
    QTextBrowser *dateString;
};

#endif // CALENDAR_H
