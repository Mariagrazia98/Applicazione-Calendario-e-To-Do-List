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
    CalendarObjectWidget(QWidget *parent, CalendarObject& calendarObject, ConnectionManager* connectionManager);
    ~CalendarObjectWidget();

private slots:
    void onModifyButtonClicked();
    void onRemoveButtonClicked();
    void finished(QNetworkReply* reply);
    signals:
    void taskModified( CalendarObject& obj);
    void taskDeleted(CalendarObject& obj);

private:
    Ui::CalendarObjectWidget *ui;
    CalendarObject* calendarObject;
    QMetaObject::Connection connectionToFinish;

    void setupUI();
    void setupText();

    QHBoxLayout* displayLayout;
    QVBoxLayout* buttonsLayout;
    QCheckBox* checkBox;
    QTextBrowser* textBrowser;
    QPushButton* modifyButton;
    QPushButton* removeButton;
    ConnectionManager* connectionManager;

    void setupButtons();
};

#endif // CALENDAROBJECTWIDGET_H
