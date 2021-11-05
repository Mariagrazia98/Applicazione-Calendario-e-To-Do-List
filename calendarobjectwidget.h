#ifndef CALENDAROBJECTWIDGET_H
#define CALENDAROBJECTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QTextBrowser>

#include "calendarobject.h"
#include "calendarevent.h"
#include "calendartodo.h"
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
    void onCheckBoxToggled(bool checked);
    void finished(QNetworkReply* reply);
    void onTaskModified();
    signals:
    void taskModified();
    void taskDeleted(CalendarObject& obj);

private:
    void setupUI();
    void setupText();
    void setupButtons();

    Ui::CalendarObjectWidget *ui;
    CalendarObject* calendarObject;
    QMetaObject::Connection connectionToFinish;

    QHBoxLayout* displayLayout;
    QVBoxLayout* buttonsLayout;
    QCheckBox* checkBox;
    QTextBrowser* textBrowser;
    QPushButton* modifyButton;
    QPushButton* removeButton;
    ConnectionManager* connectionManager;

    QNetworkAccessManager* networkAccessManager;

};

#endif // CALENDAROBJECTWIDGET_H
