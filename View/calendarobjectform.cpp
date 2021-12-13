#include "calendarobjectform.h"
#include "ui_calendarobjectform.h"
#include "../Model/calendartodo.h"

#include <iostream>

CalendarObjectForm::CalendarObjectForm(QMap<QString, std::shared_ptr<ConnectionManager>> connectionManagers,
                                       CalendarObject *calendarObject) :
        QWidget(nullptr),
        connectionManagers(connectionManagers),
        calendarObject(calendarObject),
        ui(new Ui::CalendarObjectForm) {

    /* Setup UI */

    ui->setupUi(this);
    setFixedSize(ui->formLayout->sizeHint() + ui->buttonBox->sizeHint());
    ui->numRepetition->setValue(0);
    ui->typeRepetition->setCurrentIndex(-1);
    ui->prioritySpinBox->setVisible(false);
    ui->priorityLabel->setVisible(false);
    ui->untilDate->setVisible(false);
    ui->untilLabel->setVisible(false);

    for (auto connectionManager = connectionManagers.begin();
         connectionManager != connectionManagers.end(); connectionManager++) {
        ui->calendarComboBox->addItem(connectionManager.key());
    }

    if (calendarObject) {
        /* Modify an existing calendarObject */
        ui->calendarComboBox->setCurrentText(calendarObject->getCalendarName());
        ui->calendarComboBox->setDisabled(true);
        ui->name->setText(calendarObject->getName());
        ui->description->setText((calendarObject->getDescription()));
        ui->location->setText(calendarObject->getLocation());
        ui->numRepetition->setValue(calendarObject->getNumRepetition());
        ui->typeRepetition->setCurrentIndex(calendarObject->getTypeRepetition());
        if (calendarObject->getNumRepetition() >= 1) {
            ui->untilDate->setVisible(true);
            ui->untilLabel->setVisible(true);
            ui->untilDate->setDate(calendarObject->getUntilDateRepetition());
        }
        ui->comboBox->setDisabled(true);

        if (auto parent = calendarObject->getParent().lock()) {
            /* This is a recurrence */
            ui->beginDateTime->setDateTime(parent->getStartDateTime());
        } else {
            ui->beginDateTime->setDateTime(calendarObject->getStartDateTime());
        }

        CalendarEvent *calendarEvent = dynamic_cast<CalendarEvent *>(calendarObject);
        if (calendarEvent) {
            /* The existing calendarObject the user wants to modify is an event */
            ui->comboBox->setCurrentIndex(0);
            ui->endDateTime->setDateTime(calendarEvent->getEndDateTime());
            if (auto parent = calendarObject->getParent().lock()) {
                CalendarEvent *parent_ = dynamic_cast<CalendarEvent *>(parent.get());
                ui->endDateTime->setDateTime(parent_->getEndDateTime());
            }
        } else {
            /* The existing calendarObject the user wants to modify is a task */
            CalendarToDo *calendarToDo = dynamic_cast<CalendarToDo *>(calendarObject);
            if (calendarToDo) {
                ui->comboBox->setCurrentIndex(1);
                ui->prioritySpinBox->setVisible(true);
                ui->priorityLabel->setVisible(true);
                ui->prioritySpinBox->setValue(calendarToDo->getPriority());
                ui->horizontalSpacer->changeSize(0, 0, QSizePolicy::Fixed);
                ui->endDateTime->setVisible(false);
                ui->endLabel->setVisible(false);
            }
        }
    } else {
        /* Add a new calendarObject */
        QDateTime dateTime = QDateTime::currentDateTime();
        ui->beginDateTime->setDateTime(dateTime);
        ui->endDateTime->setDateTime(dateTime.addSecs(3600));
        ui->untilDate->setDate(QDate::currentDate());
    }

    /* Set dates display format */
    QLocale locale = QLocale(QLocale::English, QLocale::UnitedKingdom); // set the locale you want here
    ui->beginDateTime->setLocale(locale);
    ui->endDateTime->setLocale(locale);
    ui->untilDate->setLocale(locale);
    ui->beginDateTime->setDisplayFormat("yyyy/MM/dd HH:mm");
    ui->endDateTime->setDisplayFormat("yyyy/MM/dd HH:mm");
    ui->untilDate->setDisplayFormat("yyyy/MM/dd");

    connect(ui->numRepetition, &QSpinBox::valueChanged, this, &CalendarObjectForm::onNumRepetitionChanged);
    connect(ui->typeRepetition, &QComboBox::currentIndexChanged, this, &CalendarObjectForm::onTypeRepetitionChanged);

}

CalendarObjectForm::~CalendarObjectForm() {
    delete ui;
}

void CalendarObjectForm::on_buttonBox_rejected() {
    this->close();
}

void CalendarObjectForm::on_buttonBox_accepted() {
    /* Selection of the connection manager which will manage the request */
    std::shared_ptr<ConnectionManager> connectionManager;
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

    /* Set the type of object and the calendar name for the request */
    calendarName = connectionManager->getCalendarDisplayName();
    QString objectType;
    if (ui->comboBox->currentIndex() == 0) {
        objectType = "VEVENT";
    } else {
        objectType = "VTODO";
    }

    /* Input validation */
    if (ui->name->text().isEmpty()) {
        QMessageBox::warning(this, "Error", "Insert a valid name");
        return;
    }
    if (ui->endDateTime->isVisible() && ui->endDateTime->dateTime() < ui->beginDateTime->dateTime()) {
        QMessageBox::warning(this, "Error", "Insert a valid start and end date");
        return;
    }
    if (ui->untilDate->isVisible() && ui->untilDate->date() < ui->endDateTime->date()) {
        QMessageBox::warning(this, "Error", "Insert a valid start and until date");
        return;
    }

    /* Composition of the request */
    QString requestString = "BEGIN:VCALENDAR\r\n"
                            "BEGIN:" + objectType + "\r\n"
                                                    "UID:" + UID + "\r\n"
                                                                   "VERSION:2.0\r\n"
                                                                   "DTSTAMP:" +
                            QDateTime::currentDateTime().toString("yyyyMMddTHHmmssZ") + "\r\n"
                                                                                        "SUMMARY:" + ui->name->text() +
                            "\r\n"
                            "DTSTART:" + ui->beginDateTime->dateTime().toString("yyyyMMddTHHmmss") + "\r\n";
    if (!ui->location->text().isEmpty()) {
        requestString.append("LOCATION:" + ui->location->text() + "\r\n");
    }

    if (!ui->description->toPlainText().isEmpty()) {
        requestString.append("DESCRIPTION:" + ui->description->toPlainText() + "\r\n");
    }
    requestString.append("TRANSP:OPAQUE\r\n");
    if (ui->typeRepetition->currentIndex() != CalendarObject::RepetitionType::NONE && ui->numRepetition->value() > 0) {
        /* Part of the request related to the repetitions */
        QString rrule = "RRULE:FREQ=";
        switch (ui->typeRepetition->currentIndex()) {
            case CalendarObject::RepetitionType::DAILY:
                rrule += "DAILY";
                break;
            case CalendarObject::RepetitionType::WEEKLY:
                rrule += "WEEKLY";
                break;
            case CalendarObject::RepetitionType::MONTHLY:
                rrule += "MONTHLY";
                break;
            case CalendarObject::RepetitionType::YEARLY:
                rrule += "YEARLY";
                break;
            default:
                break;
        }
        rrule += ";COUNT=" + QString::number(ui->numRepetition->value()) + "\r\n";
        requestString.append(rrule);
        requestString.append("UNTIL:" + ui->untilDate->date().toString("yyyyMMddT000000") + "\r\n");
    }

    if (ui->comboBox->currentIndex() == 0) {
        /* Fields of CalendarEvent */
        requestString.append("DTEND:" + ui->endDateTime->dateTime().toString("yyyyMMddTHHmmss") + "\r\n");
        requestString.append("PRIORITY:0\r\n");
    } else {
        requestString.append("PRIORITY:" + QString::number(ui->prioritySpinBox->value()) + "\r\n");
    }
    if (calendarObject) {
        std::shared_ptr<CalendarToDo> calendarToDo = std::dynamic_pointer_cast<CalendarToDo>(calendarObject);
        if (calendarToDo) {
            /* Optional fields of CalendarToDo*/
            QList<QDate> completedDates = calendarToDo->getCompletedDate();
            if (!completedDates.isEmpty()) {
                requestString.append("COMPLETED:");
                for (int i = 0; i < completedDates.size(); i++) {
                    requestString.append(completedDates[i].toString("yyyyMMddT000000"));
                    if (i != completedDates.size() - 1) {
                        requestString.append(',');
                    }
                }
                requestString.append("\r\n");
            }
        }

        /* Exception dates of CalendarEvent and CalendarToDO */
        QSet<QDate> exDates = calendarObject->getExDates();
        if (!exDates.isEmpty()) {
            requestString.append("EXDATE:");
            QSet<QDate>::const_iterator i = exDates.constBegin();
            while (i != exDates.constEnd()) {
                requestString.append(i->toString("yyyyMMddT000000"));
                ++i;
                if (i != exDates.constEnd()) {
                    requestString.append(',');
                }
            }
            requestString.append("\r\n");
        }
    }
    requestString.append("END:" + objectType + "\r\n" + "END:VCALENDAR");
    /* Composition of request end */

    /* Passing the requestString to the connectionManager which will handle the request*/
    connectionToFinish = connect(connectionManager.get(), &ConnectionManager::insertOrUpdatedCalendarObject, this,
                                 &CalendarObjectForm::handleUploadFinished);
    connectionManager->addOrUpdateCalendarObject(requestString, UID);
}

void CalendarObjectForm::handleUploadFinished(QNetworkReply *reply) {
    /* Handling response of the insert/update request */
    disconnect(connectionToFinish);
    if (reply != nullptr) {
        QNetworkReply::NetworkError error = reply->error();
        if (error == QNetworkReply::NoError) {
            /* Success */
            emit(taskUploaded(calendarName));
            close();
        } else {
            /* Error */
            QByteArray answer = reply->readAll();
            QString answerString = QString::fromUtf8(answer);
            std::cerr << answerString.toStdString() << '\n';
            QMessageBox::warning(this, "Error", "Could not create or modify selected object");
        }
        reply->deleteLater();
    } else {
        /* Null reply */
        QMessageBox::warning(this, "Error", "Something went wrong");
    }
}

void CalendarObjectForm::on_comboBox_currentIndexChanged(int index) {
    /* Makes visible or not some fields in the form based on the chosen CalendarObject type*/
    switch (index) {
        case 0:
            /* EVENT */
            ui->endLabel->setVisible(true);
            ui->endDateTime->setVisible(true);
            ui->prioritySpinBox->setVisible(false);
            ui->priorityLabel->setVisible(false);
            ui->horizontalSpacer->changeSize(40, 20, QSizePolicy::Expanding);
            break;

        case 1:
            /* TASK */
            ui->endLabel->setVisible(false);
            ui->endDateTime->setVisible(false);
            ui->prioritySpinBox->setVisible(true);
            ui->priorityLabel->setVisible(true);
            ui->horizontalSpacer->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
            break;
        default:
            break;
    }
}

void CalendarObjectForm::onNumRepetitionChanged(int i) {
    if (ui->numRepetition->value() >= 1 && ui->typeRepetition->currentIndex() != -1) {
        ui->untilDate->setVisible(true);
        ui->untilLabel->setVisible(true);
        if (ui->comboBox->currentIndex() == 0) { //Event
            ui->untilDate->setDate(ui->endDateTime->date());
        } else { //to-do
            ui->untilDate->setDate(ui->beginDateTime->date());
        }
    } else {
        ui->untilDate->setVisible(false);
        ui->untilLabel->setVisible(false);
    }
}

void CalendarObjectForm::onTypeRepetitionChanged(int i) {
    if (ui->numRepetition->value() >= 1 && ui->typeRepetition->currentIndex() != -1) {
        ui->untilDate->setVisible(true);
        ui->untilLabel->setVisible(true);
        if (ui->comboBox->currentIndex() == 0) { //Event
            ui->untilDate->setDate(ui->endDateTime->date());
        } else { //to-do
            ui->untilDate->setDate(ui->beginDateTime->date());
        }
    } else {
        ui->untilDate->setVisible(false);
        ui->untilLabel->setVisible(false);
    }
}

void CalendarObjectForm::on_beginDateTime_dateTimeChanged(const QDateTime &dateTime) {
    if (ui->comboBox->currentIndex() == 0) {
        /* Event */
        if (ui->endDateTime->dateTime() < dateTime) {
            ui->endDateTime->setDateTime(dateTime);
        }
        if (ui->untilDate->dateTime() < dateTime) {
            ui->untilDate->setDate(dateTime.date());
        }
    } else if (ui->comboBox->currentIndex() == 1 && ui->untilDate->date() < dateTime.date()) {
        /* To-Do */
        ui->untilDate->setDate(dateTime.date());
    }
}

void CalendarObjectForm::on_endDateTime_dateTimeChanged(const QDateTime &dateTime) {
    if (ui->comboBox->currentIndex() == 0 && ui->untilDate->date() < dateTime.date()) {
        /* Event */
        ui->untilDate->setDate(dateTime.date());
    }
}

void CalendarObjectForm::setDate(const QDate &date) {
    ui->beginDateTime->setDate(date);
    ui->endDateTime->setDate(date);
    ui->untilDate->setDate(date);
}

void CalendarObjectForm::closeEvent(QCloseEvent *event) {
    emit(taskFormClosed());
}