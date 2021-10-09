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

#include "../Model/calendarobject.h"
#include "../Model/calendarevent.h"
#include "../Controller/connectionManager.h"

namespace Ui {
    class TaskForm;
}

class TaskForm : public QWidget {
Q_OBJECT

public:
    TaskForm(ConnectionManager *connectionManager, CalendarObject *calendarObject = nullptr);

    ~TaskForm();

    void setDate(const QDate &date);

private
    slots:

    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

    void on_comboBox_currentIndexChanged(int index);

    void on_beginDateTime_dateTimeChanged(const QDateTime &dateTime);

    void handleUploadFinished(QNetworkReply *reply);

signals:

    void closing();

    void taskUploaded();

private:
    Ui::TaskForm *ui;
    ConnectionManager *connectionManager;
    QMetaObject::Connection connectionToFinish;

    CalendarObject *calendarObject;

    void closeEvent(QCloseEvent *event) override;
};

#endif // TASKFORM_H
