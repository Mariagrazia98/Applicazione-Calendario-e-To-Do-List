#include "taskform.h"
#include "ui_taskform.h"
#include "../Model/calendartodo.h"

#include <iostream>

TaskForm::TaskForm(QMap<QString, std::shared_ptr<ConnectionManager>> connectionManagers,
                   CalendarObject *calendarObject) :
        QWidget(nullptr),
        connectionManagers(connectionManagers),
        calendarObject(calendarObject),
        ui(new Ui::TaskForm) {

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
        ui->name->setText(calendarObject->getName());
        ui->description->setText((calendarObject->getDescription()));
        ui->location->setText(calendarObject->getLocation());
        ui->numRepetition->setValue(calendarObject->getNumRepetition());
        ui->typeRepetition->setCurrentIndex(calendarObject->getTypeRepetition());
        ui->calendarComboBox->setVisible(false);
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
            ui->expireDateTime->setDateTime(calendarEvent->getEndDateTime());
        } else {
            /* The existing calendarObject the user wants to modify is a task */
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
        /* Add a new calendarObject */
        QLocale locale = QLocale(QLocale::English, QLocale::UnitedKingdom); // set the locale you want here
        ui->beginDateTime->setDateTime(QDateTime::currentDateTime());
        ui->expireDateTime->setDateTime(QDateTime::currentDateTime());
        ui->untilDate->setDate(QDate::currentDate());
    }

    /* Set dates display format */
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
    calendarName = connectionManager->getCalendarName();
    QString objectType;
    if (ui->comboBox->currentIndex() == 0) {
        objectType = "VEVENT";
    } else {
        objectType = "VTODO";
    }

    /* Input validation */
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

    /* Composition of the request */
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
        requestString.append("UNTIL:" + ui->untilDate->date().toString("yyyyMMdd") + "\r\n");
    }

    if (ui->comboBox->currentIndex() == 0) {
        /* Fields of CalendarEvent */
        requestString.append("DTEND:" + ui->expireDateTime->dateTime().toString("yyyyMMddTHHmmss") + "\r\n");
        requestString.append("PRIORITY:0\r\n");
    } else {
        requestString.append("PRIORITY:" + QString::number(ui->prioritySpinBox->value()) + "\r\n");
        if (calendarObject) {
            std::shared_ptr<CalendarToDo> calendarToDo = std::dynamic_pointer_cast<CalendarToDo>(calendarObject);
            if (calendarToDo) {
                /* Optional fields of CalendarToDo*/
                QList<QDate> completedDates = calendarToDo->getCompletedDate();
                requestString.append("COMPLETED:");
                for (int i = 0; i < completedDates.size(); i++) {
                    requestString.append(completedDates[i].toString("yyyyMMddT010000Z"));
                    if (i != completedDates.size() - 1) {
                        requestString.append(',');
                    }
                }
                requestString.append("\r\n");
            }

            /* Exception dates of CalendarEvent and CalendarToDO */
            QSet<QDate> exDates = calendarObject->getExDates();
            requestString.append("EXDATE:");
            QSet<QDate>::const_iterator i = exDates.constBegin();
            while (i != exDates.constEnd()) {
                requestString.append(i->toString("yyyyMMddT010000Z"));
                i++;
                if (i != exDates.constEnd()) {
                    requestString.append(',');
                }
            }
            requestString.append("\r\n");
        } else {
            requestString.append("STATUS:IN-PROCESS\r\n");
        }
    }
    requestString.append("END:" + objectType + "\r\n" + "END:VCALENDAR");
    /* Composition of request end */

    /* Passing the requestString to the connectionManager which will handle the request*/
    connectionToFinish = connect(connectionManager.get(), &ConnectionManager::insertOrUpdatedCalendarObject, this,
                                 &TaskForm::handleUploadFinished);
    connectionManager->addOrUpdateCalendarObject(requestString, UID);

}


void TaskForm::handleUploadFinished(QNetworkReply *reply) {
    /* Handling response of the insert/update request */
    disconnect(connectionToFinish);
    if (reply != nullptr) {
        QByteArray answer = reply->readAll();
        QString answerString = QString::fromUtf8(answer);
        QNetworkReply::NetworkError error = reply->error();
        if (error == QNetworkReply::NoError) {
            /* Success */
            emit(taskUploaded(calendarName));
            close();
        } else {
            /* Error */
            std::cerr << error << '\n';
            QMessageBox::warning(this, "Error", "Could not create or modify selected object");
        }
        reply->deleteLater();
    } else {
        /* Null reply */
        QMessageBox::warning(this, "Error", "Something went wrong");
    }
}

void TaskForm::on_comboBox_currentIndexChanged(int index) {
    /* Makes visible or not some fields in the form based on the chosen CalendarObject type*/
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
    emit(taskFormClosed());
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