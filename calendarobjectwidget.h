#ifndef CALENDAROBJECTWIDGET_H
#define CALENDAROBJECTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QTextBrowser>

#include "calendarobject.h"
#include "calendarevent.h"

namespace Ui {
class calendarObjectWidget;
}

class calendarObjectWidget : public QWidget
{
    Q_OBJECT

public:
    calendarObjectWidget(QWidget *parent, CalendarObject& calendarObject);
    ~calendarObjectWidget();

private:
    Ui::calendarObjectWidget *ui;
    CalendarObject* calendarObject;

    void setupUI();
    void setupText();

    QHBoxLayout* displayLayout;
    QVBoxLayout* buttonsLayout;
    QCheckBox* checkBox;
    QTextBrowser* textBrowser;
    QPushButton* modifyButton;
    QPushButton* removeButton;
};

#endif // CALENDAROBJECTWIDGET_H
