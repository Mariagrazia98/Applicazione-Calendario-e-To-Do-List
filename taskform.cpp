#include "taskform.h"
#include "ui_taskform.h"


#include <iostream>

TaskForm::TaskForm(ConnectionManager *connectionManager, CalendarObject *calendarObject) :
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
        CalendarEvent *calendarEvent = dynamic_cast<CalendarEvent *>(calendarObject);
        if (calendarEvent) {
            ui->comboBox->setCurrentIndex(0);
            ui->expireDateTime->setDateTime(calendarEvent->getEndDateTime());
            ui->beginDateTime->setDateTime(calendarEvent->getStartDateTime());
        } else {
            ui->comboBox->setCurrentIndex(1);
            ui->expireLabel->setText("To complete");
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

    QString objectType;
    if (ui->comboBox->currentIndex() == 0) {
        objectType = "VEVENT";
    } else {
        objectType = "VTODO";
    }
    QString requestString = "BEGIN:VCALENDAR\r\n"
                            "BEGIN:" + objectType + "\r\n"
                                                    "UID:" + UID + "\r\n"
                                                                   "VERSION:2.0\r\n"
                                                                   "DTSTAMP:" +
                            QDateTime::currentDateTime().toString("yyyyMMddTHHmmssZ") +
                            "\r\n"
                            "SUMMARY:" + ui->name->text() + "\r\n"
                                                            "DTSTART:" +
                            ui->beginDateTime->dateTime().toString("yyyyMMddTHHmmss") + "\r\n"

                                                                                        "LOCATION:" +
                            ui->location->text() + "\r\n"
                                                   "DESCRIPTION:" + ui->description->toPlainText() + "\r\n"
                                                                                                     "TRANSP:OPAQUE\r\n";
    // TODO: campi opzionali

    if (ui->comboBox->currentIndex() == 0) {
        requestString.append("DTEND:" + ui->expireDateTime->dateTime().toString("yyyyMMddTHHmmss") + "\r\n");
    } else {
        requestString.append("DUE:" + ui->expireDateTime->dateTime().toString("yyyyMMddTHHmmss") + "\r\n");
        requestString.append("PRIORITY:0\r\n");
        requestString.append("STATUS:IN-PROCESS\r\n");
    }
    /*
     if (!rrule.isEmpty())
    {
        requestString.append("RRULE:" + rrule + "\r\n");
    }


    if (!exdate.isEmpty())
    {
        requestString.append("EXDATE:" + exdate + "\r\n");
    }
     */
    requestString.append("END:" + objectType + "\r\n" + "END:VCALENDAR");

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
        std::cerr << answerString.toStdString() << '\n';
    } else {
        emit(taskUploaded());
        close();
    }

}

void TaskForm::on_comboBox_currentIndexChanged(int index) {
    switch (index) {
        case 0:
            //ui->expireDateTime->setEnabled(true);
            //ui->beginDateTime->setEnabled(true);
            break;
        case 1:
            // task
            //ui->expireDateTime->setEnabled(false);
            //ui->beginDateTime->setEnabled(false);
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
