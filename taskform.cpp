#include "taskform.h"
#include "ui_taskform.h"


#include <iostream>

TaskForm::TaskForm(ConnectionManager* connectionManager, CalendarObject *calendarObject) :
        QWidget(nullptr),
        connectionManager(connectionManager),
        calendarObject(calendarObject),
        ui(new Ui::TaskForm) {
    ui->setupUi(this);
    ui->beginDateTime->setDateTime(QDateTime::currentDateTime());
    ui->expireDateTime->setDateTime(QDateTime::currentDateTime());
    if (calendarObject) {
        ui->name->setText(calendarObject->getName());
        ui->description->setText((calendarObject->getDescription()));
        ui->location->setText(calendarObject->getLocation());
        CalendarEvent *calendarEvent = static_cast<CalendarEvent *>(calendarObject);
        if (calendarEvent) {
            ui->comboBox->setCurrentIndex(0);
            ui->expireDateTime->setDateTime(calendarEvent->getEndDateTime());
            ui->beginDateTime->setDateTime(calendarEvent->getStartDateTime());
        }
    }

}

TaskForm::~TaskForm() {
    delete ui;
}

void TaskForm::on_buttonBox_rejected() {
    this->close();
}

void TaskForm::on_buttonBox_accepted() {
    QString UID;
    if (calendarObject) {
        UID = calendarObject->getUID();
    } else {
        UID = QDateTime::currentDateTime().toString("yyyyMMdd-HHMM-00ss") + "-0000-" +
              ui->beginDateTime->dateTime().toString("yyyyMMddHHMM");
    }


    QString requestString = "BEGIN:VCALENDAR\r\n"
                            "BEGIN:VEVENT\r\n"
                            "UID:" + UID + "\r\n"
                                           "VERSION:2.0\r\n"
                                           "DTSTAMP:" + QDateTime::currentDateTime().toString("yyyyMMddTHHmmssZ") +
                            "\r\n"
                            "SUMMARY:" + ui->name->text() + "\r\n"
                                                            "DTSTART:" +
                            ui->beginDateTime->dateTime().toString("yyyyMMddTHHmmss") + "\r\n"
                                                                                        "DTEND:" +
                            ui->expireDateTime->dateTime().toString("yyyyMMddTHHmmss") + "\r\n"
                                                                                         "LOCATION:" +
                            ui->location->text() + "\r\n"
                                                   "DESCRIPTION:" + ui->description->toPlainText() + "\r\n"
                                                                                                     "TRANSP:OPAQUE\r\n";
    /* TODO: campi opzionali
     if (!rrule.isEmpty())
    {
        requestString.append("RRULE:" + rrule + "\r\n");
    }

    if (!exdate.isEmpty())
    {
        requestString.append("EXDATE:" + exdate + "\r\n");
    }
     */
    requestString.append("END:VEVENT\r\nEND:VCALENDAR");

    connectionToFinish = connect(connectionManager, &ConnectionManager::finished, this,
                                 &TaskForm::handleUploadFinished);
    connectionManager->addOrUpdateCalendarObject(requestString, UID);

}

void TaskForm::handleUploadFinished(QNetworkReply *reply) {
    disconnect(connectionToFinish);
    QByteArray answer = reply->readAll();
    QString answerString = QString::fromUtf8(answer);

    QNetworkReply::NetworkError error = reply->error();
    const QString &errorString = reply->errorString();
    if (error != QNetworkReply::NoError) {
        QMessageBox::warning(this, "Error", errorString);
    } else {
        emit(taskUploaded());
        close();
    }

}

void TaskForm::on_comboBox_currentIndexChanged(int index) {
    switch (index) {
        case 0:
            ui->expireDateTime->setEnabled(true);
            ui->beginDateTime->setEnabled(true);
            break;
        case 1:
            // task
            ui->expireDateTime->setEnabled(false);
            ui->beginDateTime->setEnabled(false);
            break;
        default:
            break;
    }
}

void TaskForm::on_beginDateTime_dateTimeChanged(const QDateTime &dateTime) {
    ui->expireDateTime->setDateTime(dateTime);
}

void TaskForm::closeEvent(QCloseEvent *event) {
    emit(closing());
}
