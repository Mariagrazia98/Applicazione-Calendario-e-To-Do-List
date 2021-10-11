#ifndef CALENDAROBJECTWIDGET_H
#define CALENDAROBJECTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QTextBrowser>

#include "../Model/calendarobject.h"
#include "../Model/calendarevent.h"
#include "../Model/calendartodo.h"
#include "taskform.h"
#include "eliminationtaskdialog.h"
#include "calendarwidget.h"

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
    void manageResponse(QNetworkReply* reply);
    void onTaskModified();
    void handleDeleteReccurrencies(int type);
    signals:
    void taskModified();
    void taskDeleted(CalendarObject& obj);

private:
    void setupUI();
    void setupText();
    void setupButtons();

    void deleteCalendarObject();

    Ui::CalendarObjectWidget *ui;
    CalendarObject* calendarObject;
    QMetaObject::Connection connectionToFinish;
    QMetaObject::Connection connectionToObjectModified;

    QHBoxLayout* displayLayout;
    QVBoxLayout* buttonsLayout;
    QCheckBox* checkBox;
    QTextBrowser* textBrowser;
    QPushButton* modifyButton;
    QPushButton* removeButton;
    ConnectionManager* connectionManager;

};

#endif // CALENDAROBJECTWIDGET_H
