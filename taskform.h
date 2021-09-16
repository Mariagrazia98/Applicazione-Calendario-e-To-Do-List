#ifndef TASKFORM_H
#define TASKFORM_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QBoxLayout>
#include <QGroupBox>
#include <QNetworkAccessManager>
#include <QBuffer>
#include <QMessageBox>
#include <QNetworkReply>
#include <QAuthenticator>

#include "calendarobject.h"
#include "calendarevent.h"

namespace Ui {
class TaskForm;
}

class TaskForm : public QWidget
{
    Q_OBJECT

public:
    explicit TaskForm(QWidget *parent = nullptr, CalendarObject* calendarObject = nullptr);
    ~TaskForm();

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void on_comboBox_currentIndexChanged(int index);
    void on_beginDateTime_dateTimeChanged(const QDateTime &dateTime);

    void handleUploadFinished(QNetworkReply* reply);
    void authenticationRequired(QNetworkReply *, QAuthenticator *);

    signals:
    void closing();

private:
    Ui::TaskForm *ui;
    QNetworkAccessManager* networkAccessManager;
    QNetworkReply * qNetworkReply;

    void closeEvent(QCloseEvent *event) override;
};

#endif // TASKFORM_H
