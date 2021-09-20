#include "taskform.h"
#include "ui_taskform.h"


#include <iostream>

TaskForm::TaskForm(QWidget *parent, CalendarObject *calendarObject) :
        QWidget(parent),
        //connectionManager(connectionManager),
        networkAccessManager(new QNetworkAccessManager),
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
    QString uid;
    if (calendarObject) {
        uid = calendarObject->getUID();
    } else {
        uid = QDateTime::currentDateTime().toString("yyyyMMdd-HHMM-00ss") + "-0000-" +
              ui->beginDateTime->dateTime().toString("yyyyMMddHHMM");
    }

    QBuffer *buffer = new QBuffer();

    buffer->open(QIODevice::ReadWrite);
    QString filename = uid + ".ics";
    QString requestString = "BEGIN:VCALENDAR\r\n"
                            "BEGIN:VEVENT\r\n"
                            "UID:" + uid + "\r\n"
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


    int buffersize = buffer->write(requestString.toUtf8());
    buffer->seek(0);
    buffer->size();

    QByteArray contentlength;
    contentlength.append(buffersize);

    QNetworkRequest request;
    request.setUrl(QUrl("http://localhost/progettopds/calendarserver.php/calendars/admin/default/" + filename));
    /*todo: DEFAULT nome cartella calendario*/

    request.setRawHeader("Content-Type", "text/calendar; charset=utf-8");
    request.setRawHeader("Content-Length", contentlength);

    qNetworkReply = networkAccessManager->put(request, buffer);

    if (qNetworkReply) {
        /*connect(qNetworkReply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(handleUploadHTTPError())); */

        connect(networkAccessManager, SIGNAL(finished(QNetworkReply * )),
                this, SLOT(handleUploadFinished(QNetworkReply * )));
        connect(networkAccessManager, SIGNAL(authenticationRequired(QNetworkReply * , QAuthenticator * )),
                this, SLOT(authenticationRequired(QNetworkReply * , QAuthenticator * )));
        //m_UploadRequestTimeoutTimer.start(m_RequestTimeoutMS);
    } else {
        //QDEBUG << m_DisplayName << ": " << "ERROR: Invalid reply pointer when requesting URL.";
        //emit error("Invalid reply pointer when requesting URL.");
        QMessageBox::warning(this, "error", "something went wrong");
    }

}

void TaskForm::handleUploadFinished(QNetworkReply *reply) {
    QByteArray answer = reply->readAll();
    QString answerString = QString::fromUtf8(answer);

    QNetworkReply::NetworkError error = reply->error();
    const QString &errorString = reply->errorString();
    if (error != QNetworkReply::NoError) {
        QMessageBox::warning(this, "Error", errorString);
    } else {
        //QMessageBox::information(this, "Login", "Upload succesfull");
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

void TaskForm::authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator) {
    authenticator->setUser("admin");
    authenticator->setPassword("admin");
}

void TaskForm::closeEvent(QCloseEvent *event) {
    emit(closing());
}
