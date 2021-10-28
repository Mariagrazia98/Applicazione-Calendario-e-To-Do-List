#include "taskform.h"
#include "ui_taskform.h"
#include "../Model/calendartodo.h"

#define DAILY 1
#define WEEKLY 2
#define MONTHLY 3
#define YEARLY 4

#include <iostream>

TaskForm::TaskForm(QMap<QString, std::shared_ptr<ConnectionManager *>> connectionManagers,
                   CalendarObject *calendarObject) :
        QWidget(nullptr),
        connectionManagers(connectionManagers),
        calendarObject(calendarObject),
        ui(new Ui::TaskForm) {
    ui->setupUi(this);
    setFixedSize(ui->formLayout->sizeHint() + ui->buttonBox->sizeHint());
    ui->numRepetition->setValue(0);
    ui->typeRepetition->setCurrentIndex(-1);
    ui->prioritySpinBox->setVisible(false);
    ui->priorityLabel->setVisible(false);
    ui->untilDate->setVisible(false);
    ui->untilLabel->setVisible(false);
    for(auto connectionManager = connectionManagers.begin(); connectionManager!=connectionManagers.end(); connectionManager++){
        ui->calendarComboBox->addItem(connectionManager.key());
    }
    /* MODIFY */
    if (calendarObject) {
        ui->name->setText(calendarObject->getName());
        ui->description->setText((calendarObject->getDescription()));
        ui->location->setText(calendarObject->getLocation());
        ui->numRepetition->setValue(calendarObject->getNumRepetition());
        ui->typeRepetition->setCurrentIndex(calendarObject->getTypeRepetition());
        ui->calendarComboBox->setVisible(false);
        if (calendarObject->getNumRepetition() >= 1) {
            ui->untilDate->setVisible(true);
            ui->untilLabel->setVisible(true);
            ui->untilDate->setDate(calendarObject->getUntilDateRipetition());
        }
        ui->comboBox->setDisabled(true);

        if (calendarObject->getParent()) {
            // this is a reccurrence
            ui->beginDateTime->setDateTime((*calendarObject->getParent())->getStartDateTime());
        } else {
            ui->beginDateTime->setDateTime(calendarObject->getStartDateTime());
        }
        //TODO: setCurrentIndex based on typeRepetitionn of calendarObject
        CalendarEvent *calendarEvent = dynamic_cast<CalendarEvent *>(calendarObject);
        if (calendarEvent) {
            ui->comboBox->setCurrentIndex(0);
            ui->expireDateTime->setDateTime(calendarEvent->getEndDateTime());
        } else {
            CalendarToDo *calendarToDo = dynamic_cast<CalendarToDo *>(calendarObject);
            if (calendarToDo) {
                ui->comboBox->setCurrentIndex(1);
                ui->prioritySpinBox->setVisible(true);
                ui->priorityLabel->setVisible(true);
                ui->prioritySpinBox->setValue(calendarToDo->getPriority());
                ui->horizontalSpacer->changeSize(0, 0, QSizePolicy::Fixed);
                ui->expireDateTime->setVisible(false);
                ui->expireLabel->setVisible(false);
            }
        }
    } else {
        QLocale locale = QLocale(QLocale::English, QLocale::UnitedKingdom); // set the locale you want here
        ui->beginDateTime->setDateTime(QDateTime::currentDateTime());
        ui->expireDateTime->setDateTime(QDateTime::currentDateTime());
        ui->untilDate->setDate(QDate::currentDate());
    }
    QLocale locale = QLocale(QLocale::English, QLocale::UnitedKingdom); // set the locale you want here
    ui->beginDateTime->setLocale(locale);
    ui->expireDateTime->setLocale(locale);
    ui->untilDate->setLocale(locale);
    ui->beginDateTime->setDisplayFormat("yyyy/MM/dd HH:mm");
    ui->expireDateTime->setDisplayFormat("yyyy/MM/dd HH:mm");
    ui->untilDate->setDisplayFormat("yyyy/MM/dd");

    connect(ui->numRepetition, &QSpinBox::valueChanged, this, &TaskForm::onNumRepetitionChanged);
}

TaskForm::~TaskForm() {
    delete ui;
}

void TaskForm::on_buttonBox_rejected() {
    this->close();
}

void TaskForm::on_buttonBox_accepted() {
    std::shared_ptr<ConnectionManager *>connectionManager;
    QString UID;
    if (calendarObject) {
        UID = calendarObject->getUID();
        connectionManager = connectionManagers[calendarObject->getCalendarName()];
    } else {
        const int index = ui->calendarComboBox->currentIndex();
        connectionManager = connectionManagers[connectionManagers.keys()[index]];
        UID = QDateTime::currentDateTime().toString("yyyyMMdd-HHMM-00ss") + "-0000-" +
              ui->beginDateTime->dateTime().toString("yyyyMMddHHMM");
    }
    calendarName = (*connectionManager.get())->getCalendarName();

    QString objectType;
    if (ui->comboBox->currentIndex() == 0) {
        objectType = "VEVENT";
    } else {
        objectType = "VTODO";
    }

    if (ui->name->text().isEmpty()) {
        std::cerr << "Insert a valid name\n";
        QMessageBox::warning(this, "Error", "Insert a valid name");
        return;
    }
    if (ui->expireDateTime->dateTime() < ui->beginDateTime->dateTime()) {
        std::cerr << "Insert a valid start and end date\n";
        QMessageBox::warning(this, "Error", "Insert a valid start and end date");
        return;
    }

    QString requestString = "BEGIN:VCALENDAR\r\n"
                            "BEGIN:" + objectType + "\r\n"
                                                    "UID:" + UID + "\r\n"
                                                                   "VERSION:2.0\r\n"
                                                                   "DTSTAMP:" +
                            QDateTime::currentDateTime().toString("yyyyMMddTHHmmssZ") + "\r\n"
                                                                                        "SUMMARY:" + ui->name->text() +
                            "\r\n"
                            "DTSTART:" + ui->beginDateTime->dateTime().toString("yyyyMMddTHHmmss") + "\r\n"
                                                                                                     "LOCATION:" +
                            ui->location->text() + "\r\n"
                                                   "DESCRIPTION:" + ui->description->toPlainText() + "\r\n"
                                                                                                     "TRANSP:OPAQUE\r\n";
    if (ui->typeRepetition->currentIndex() > 0 && ui->numRepetition->value() != 0) {
        QString rrule = "RRULE:FREQ=";
        switch (ui->typeRepetition->currentIndex()) {
            case DAILY:
                rrule += "DAILY";
                break;
            case WEEKLY:
                rrule += "WEEKLY";
                break;
            case MONTHLY:
                rrule += "MONTHLY";
                break;
            case YEARLY:
                rrule += "YEARLY";
                break;
            default:
                break;
        }
        rrule += ";COUNT=" + QString::number(ui->numRepetition->value()) + "\r\n";
        requestString.append(rrule);
        requestString.append("UNTIL:" + ui->untilDate->date().toString("yyyyMMdd") + "\r\n");
    }

    // TODO: campi opzionali
    if (ui->comboBox->currentIndex() == 0) { //Event
        requestString.append("DTEND:" + ui->expireDateTime->dateTime().toString("yyyyMMddTHHmmss") + "\r\n");
        requestString.append("PRIORITY:0\r\n");
    } else {
        requestString.append("PRIORITY:" + QString::number(ui->prioritySpinBox->value()) + "\r\n");
        if (calendarObject) {
            CalendarToDo *calendarToDo = dynamic_cast<CalendarToDo *>(calendarObject);
            if (calendarToDo->getCompletedDateTime()) {
                requestString.append(
                        "COMPLETED:" + calendarToDo->getCompletedDateTime()->toString("yyyyMMddTHHmmss") + "\r\n");
                requestString.append("STATUS:COMPLETED\r\n");
            }
        } else {
            requestString.append("STATUS:IN-PROCESS\r\n");
        }
    }
    requestString.append("END:" + objectType + "\r\n" + "END:VCALENDAR");



    connectionToFinish = connect(*connectionManager.get(), &ConnectionManager::insertOrUpdatedCalendarObject, this,
                                 &TaskForm::handleUploadFinished);
    (*connectionManager.get())->addOrUpdateCalendarObject(requestString, UID);

}


void TaskForm::handleUploadFinished(QNetworkReply *reply) {
    disconnect(connectionToFinish);
    if (reply != nullptr) {
        QByteArray answer = reply->readAll();
        QString answerString = QString::fromUtf8(answer);
        QNetworkReply::NetworkError error = reply->error();
        if (error == QNetworkReply::NoError) {
            emit(taskUploaded(calendarName));
            close();
        } else {
            //const QString &errorString = reply->errorString();
            std::cerr << error << '\n';
            QMessageBox::warning(this, "Error", "Could not create or modify selected object");
        }
        reply->deleteLater();
    } else {
        // null reply
        QMessageBox::warning(this, "Error", "Something went wrong");
    }

}

void TaskForm::on_comboBox_currentIndexChanged(int index) {
    switch (index) {
        case 0:
            /* EVENT */
            ui->expireLabel->setVisible(true);
            ui->expireDateTime->setVisible(true);
            ui->prioritySpinBox->setVisible(false);
            ui->priorityLabel->setVisible(false);
            ui->horizontalSpacer->changeSize(40, 20, QSizePolicy::Expanding);
            break;
        case 1:
            /* TASK */
            ui->expireLabel->setVisible(false);
            ui->expireDateTime->setVisible(false);
            ui->prioritySpinBox->setVisible(true);
            ui->priorityLabel->setVisible(true);
            ui->horizontalSpacer->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
            break;
        default:
            break;
    }
}

void TaskForm::on_beginDateTime_dateTimeChanged(const QDateTime &dateTime) {
    if (ui->expireDateTime->dateTime() < dateTime) {
        ui->expireDateTime->setDateTime(dateTime);
    }
}

void TaskForm::closeEvent(QCloseEvent *event) {
    emit(closing());
}

void TaskForm::setDate(const QDate &date) {
    ui->beginDateTime->setDate(date);
    ui->expireDateTime->setDate(date);
    ui->untilDate->setDate(date);
}

void TaskForm::onNumRepetitionChanged(int i) {
    if (ui->numRepetition->value() >= 1) {
        ui->untilDate->setVisible(true);
        ui->untilLabel->setVisible(true);
    } else {
        ui->untilDate->setVisible(false);
        ui->untilLabel->setVisible(false);
    }
}
