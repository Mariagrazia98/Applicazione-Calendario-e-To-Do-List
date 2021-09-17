#ifndef CALENDAROBJECTWIDGET_H
#define CALENDAROBJECTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QTextBrowser>

#include "calendarobject.h"
#include "calendarevent.h"
#include "taskform.h"

namespace Ui {
class CalendarObjectWidget;
}

class CalendarObjectWidget : public QWidget
{
    Q_OBJECT

public:
    CalendarObjectWidget(QWidget *parent, CalendarObject& calendarObject);
    ~CalendarObjectWidget();

private slots:
    void onModifyButtonClicked();
    void onRemoveButtonClicked();


    signals:
    void taskModified( CalendarObject& obj);
private:
    Ui::CalendarObjectWidget *ui;
    CalendarObject* calendarObject;

    void setupUI();
    void setupText();

    QHBoxLayout* displayLayout;
    QVBoxLayout* buttonsLayout;
    QCheckBox* checkBox;
    QTextBrowser* textBrowser;
    QPushButton* modifyButton;
    QPushButton* removeButton;

    void setupButtons();
};

#endif // CALENDAROBJECTWIDGET_H
