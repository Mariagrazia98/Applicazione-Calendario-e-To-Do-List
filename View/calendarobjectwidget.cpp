#include "calendarobjectwidget.h"
#include "ui_calendarobjectwidget.h"
#include "eliminationtaskdialog.h"

#include <iostream>

#define DAILY 1
#define WEEKLY 2
#define MONTHLY 3
#define YEARLY 4

CalendarObjectWidget::CalendarObjectWidget(QWidget *parent, CalendarObject &calendarObject,
                                           ConnectionManager *connectionManager) :
        QWidget(parent),
        calendarObject(&calendarObject),
        displayLayout(new QHBoxLayout),
        buttonsLayout(new QVBoxLayout),
        checkBox(new QCheckBox),
        textBrowser(new QTextBrowser),
        modifyButton(new QPushButton(this)),
        removeButton(new QPushButton(this)),
        connectionManager(connectionManager),
        ui(new Ui::CalendarObjectWidget) {
    ui->setupUi(this);
    setupUI();
}

CalendarObjectWidget::~CalendarObjectWidget() {
    delete ui;
}


void CalendarObjectWidget::setupUI() {
    CalendarEvent *calendarEvent = dynamic_cast<CalendarEvent *>(calendarObject);
    checkBox->adjustSize();
    if (calendarEvent) {
        checkBox->setVisible(false);
        displayLayout->addSpacing(17);
    } else {
        CalendarToDo *calendarToDO = dynamic_cast<CalendarToDo *>(calendarObject);
        if (calendarToDO->getCompletedDateTime()) {
            checkBox->setCheckState(Qt::Checked);
        } else {
            checkBox->setCheckState(Qt::Unchecked);
        }
        checkBox->setVisible(true);
        displayLayout->addWidget(checkBox);
        connect(checkBox, &QCheckBox::stateChanged, this, &CalendarObjectWidget::onCheckBoxToggled);
    }

    setupText();
    displayLayout->addWidget(textBrowser);
    setupButtons();
    this->setLayout(displayLayout);
}

void CalendarObjectWidget::setupText() {
    QString text;
    text.append("Name: " + calendarObject->getName() + '\n');
    if (!calendarObject->getLocation().isEmpty()) {
        text.append("Location: " + calendarObject->getLocation() + '\n');
    }
    if (!calendarObject->getDescription().isEmpty()) {
        text.append("Description: " + calendarObject->getDescription() + '\n');
    }
    CalendarEvent *calendarEvent = dynamic_cast<CalendarEvent *>(calendarObject);
    if (calendarEvent != nullptr) {
        // calendarObject is a CalendarEvent
        text.append(
                "Start date and time: " + calendarEvent->getStartDateTime().toString("dddd, yyyy/MM/d hh:mm") + '\n');
        text.append("End date and time: " + calendarEvent->getEndDateTime().toString("dddd, yyyy/MM/d hh:mm") + '\n');
    } else {
        CalendarToDo *calendarToDo = dynamic_cast<CalendarToDo *>(calendarObject);
        if (calendarToDo != nullptr) {
            // calendarObject is a CalendarEvent
            if (calendarToDo->getCompletedDateTime()) {
                textBrowser->setTextColor(QColor(0, 150, 0));
            }
            if (calendarToDo->getStartDateTime()) {
                text.append(
                        "Start date and time: " + calendarToDo->getStartDateTime()->toString("dddd, yyyy/MM/d hh:mm") +
                        '\n');
            }
            if (calendarToDo->getDueDateTime()) {
                text.append(
                        "Due date and time: " + calendarToDo->getDueDateTime()->toString("dddd, yyyy/MM/d hh:mm") +
                        '\n');
            }

        }
    }
    textBrowser->setText(text);
}


void CalendarObjectWidget::setupButtons() {
    modifyButton->setIcon(QIcon(":/resources/edit.png"));
    if (modifyButton->icon().isNull()) {
        modifyButton->setText("Modify");
    }
    modifyButton->setToolTip("Modify");
    buttonsLayout->addWidget(modifyButton);
    removeButton->setIcon(QIcon(":/resources/garbage.png"));
    if (removeButton->icon().isNull()) {
        removeButton->setText("Remove");
    }
    removeButton->setToolTip("Remove");
    buttonsLayout->addWidget(removeButton);
    displayLayout->addLayout(buttonsLayout);

    connect(modifyButton, &QPushButton::clicked, this, &CalendarObjectWidget::onModifyButtonClicked);
    connect(removeButton, &QPushButton::clicked, this, &CalendarObjectWidget::onRemoveButtonClicked);
}

void CalendarObjectWidget::onModifyButtonClicked() {
    TaskForm *taskForm = new TaskForm(connectionManager, calendarObject);
    taskForm->show();
    connectionToObjectModified = connect(taskForm, &TaskForm::taskUploaded, this,
                                         &CalendarObjectWidget::onTaskModified);
}

void CalendarObjectWidget::onRemoveButtonClicked() {
    if (calendarObject->getTypeRepetition() > 0 && calendarObject->getNumRepetition() != 0) {
        // has repetitions
        EliminationTaskDialog *dialog = new EliminationTaskDialog(nullptr);
        dialog->show();
        connect(dialog, &EliminationTaskDialog::eliminateRecurrences, this,
                &CalendarObjectWidget::handleDeleteReccurrencies);
    } else {
        deleteCalendarObject();
    }
}

void CalendarObjectWidget::handleDeleteReccurrencies(int type) {
    if (type == 0) { // delete all recurrences
        deleteCalendarObject();
    } else if (type == 1) { // delete only one recurrence

        QString objectType;
        CalendarToDo *calendarToDo = dynamic_cast<CalendarToDo *>(calendarObject);
        if (calendarToDo) {
            objectType = "VTODO";
        } else {
            objectType = "VEVENT";
        }

        QString requestString =
                "BEGIN:VCALENDAR\r\nBEGIN:" + objectType + "\r\nUID:" + calendarObject->getUID() +
                "\r\nVERSION:2.0\r\nDTSTAMP:" +
                calendarObject->getCreationDateTime().toString("yyyyMMddTHHmmssZ") +
                "\r\nSUMMARY:" + calendarObject->getName() + "\r\nLOCATION:" +
                calendarObject->getLocation() + "\r\nDESCRIPTION:" + calendarObject->getDescription() +
                "\r\nTRANSP:OPAQUE\r\n";


        if (calendarToDo) {
            if (calendarToDo->getStartDateTime()) {
                requestString.append(
                        "DTSTART:" + calendarToDo->getStartDateTime()->toString("yyyyMMddTHHmmss") + "\r\n");
            }
            if (calendarToDo->getDueDateTime()) {
                requestString.append("DUE:" + calendarToDo->getDueDateTime()->toString("yyyyMMddTHHmmss") + "\r\n");
            }

            if (calendarToDo->getCompletedDateTime()) {
                requestString.append(
                        "COMPLETED:" + calendarToDo->getCompletedDateTime()->toString("yyyyMMddTHHmmss") + "\r\n");
                requestString.append("STATUS:COMPLETED\r\n");
            } else {
                requestString.append("STATUS:IN-PROCESS\r\n");
            }

            QDate recurrenceDate;
            if (calendarToDo->getStartDateTime()) {
                recurrenceDate = calendarToDo->getStartDateTime()->date();
            } else {
                recurrenceDate = calendarToDo->getCreationDateTime().date();
            }
            requestString.append("EXDATE:");
            std::cout << "deleting ToDo reccurence on " +
                         recurrenceDate.toString("yyyyMMddT010000Z").toStdString() + '\n';
            requestString.append(recurrenceDate.toString("yyyyMMddT010000Z") + "\r\n");
        } else {
            CalendarEvent *calendarEvent = dynamic_cast<CalendarEvent *>(calendarObject);
            if (calendarEvent) {
                requestString.append("EXDATE:");
                std::cout << "deleting Event reccurence on " +
                             calendarEvent->getStartDateTime().toString("yyyyMMddT010000Z").toStdString() + '\n';
                requestString.append(calendarEvent->getStartDateTime().toString("yyyyMMddT010000Z") + "\r\n");
            }
        }

        if (calendarObject->getTypeRepetition() > 0 && calendarObject->getNumRepetition() != 0) {
            QString rrule = "RRULE:FREQ=";
            switch (calendarObject->getTypeRepetition()) {
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
            rrule += ";COUNT=" + QString::number(calendarObject->getNumRepetition()) + "\r\n";
            requestString.append(rrule);
        }


        requestString.append("PRIORITY:" + QString::number(calendarObject->getPriority()) + "\r\n");

        requestString.append("END:" + objectType + "\r\nEND:VCALENDAR");

        connectionToFinish = connect(connectionManager, &ConnectionManager::onFinished, this,
                                     &CalendarObjectWidget::manageResponse);
        connectionManager->addOrUpdateCalendarObject(requestString, calendarObject->getUID());
    }
}

void CalendarObjectWidget::deleteCalendarObject() {
    connectionToFinish = connect(connectionManager, SIGNAL(objectDeleted(QNetworkReply * )), this,
                                 SLOT(manageResponse(QNetworkReply * )));
    connectionManager->deleteCalendarObject(calendarObject->getUID());
}

void CalendarObjectWidget::manageResponse(QNetworkReply *reply) {
    disconnect(connectionToFinish);
    if (reply != nullptr) {
        QByteArray answer = reply->readAll();
        QString answerString = QString::fromUtf8(answer);
        QNetworkReply::NetworkError error = reply->error();
        if (error == QNetworkReply::NoError) {
            emit(taskDeleted(*calendarObject));
        } else {
            const QString &errorString = reply->errorString();
            std::cerr << error << "\n";
            QMessageBox::warning(this, "Error", errorString);
        }
    } else {
        // reply is null
        QMessageBox::warning(this, "Error", "Something went wrong");
    }

}

void CalendarObjectWidget::onTaskModified() {
    disconnect(connectionToObjectModified);
    emit(taskModified());
}

void CalendarObjectWidget::onCheckBoxToggled(bool checked) {
    CalendarToDo *calendarToDo = dynamic_cast<CalendarToDo *>(calendarObject);
    if (checked) {
        calendarToDo->setCompletedDateTime(QDateTime::currentDateTime());
    } else {
        calendarToDo->getCompletedDateTime().reset();
    }

    QString requestString = "BEGIN:VCALENDAR\r\n"
                            "BEGIN:VTODO\r\n"
                            "UID:" + calendarObject->getUID() + "\r\n"
                                                                "VERSION:2.0\r\n"
                                                                "DTSTAMP:" +
                            calendarObject->getCreationDateTime().toString("yyyyMMddTHHmmssZ") +
                            "\r\n"
                            "SUMMARY:" + calendarObject->getName() + "\r\n"
                                                                     "DTSTART:" +
                            calendarToDo->getStartDateTime()->toString("yyyyMMddTHHmmss") + "\r\n"

                                                                                            "LOCATION:" +
                            calendarObject->getLocation() + "\r\n"
                                                            "DESCRIPTION:" + calendarObject->getDescription() + "\r\n"
                                                                                                                "TRANSP:OPAQUE\r\n";

    requestString.append("DUE:" + calendarToDo->getDueDateTime()->toString("yyyyMMddTHHmmss") + "\r\n");
    if (calendarToDo->getCompletedDateTime()) {
        requestString.append("COMPLETED:" + calendarToDo->getCompletedDateTime()->toString("yyyyMMddTHHmmss") + "\r\n");
        requestString.append("STATUS:COMPLETED\r\n");
    } else {
        requestString.append("STATUS:IN-PROCESS\r\n");
    }

    if (calendarObject->getTypeRepetition() > 0 && calendarObject->getNumRepetition() != 0) {
        QString rrule = "RRULE:FREQ=";
        switch (calendarObject->getTypeRepetition()) {
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
        rrule += ";COUNT=" + QString::number(calendarObject->getNumRepetition()) + "\r\n";
        requestString.append(rrule);
    }

    if (calendarToDo->getDueDateTime()) {
        requestString.append("DUE:" + calendarToDo->getDueDateTime()->toString("yyyyMMddTHHmmss") + "\r\n");
    }

    requestString.append("PRIORITY:" + QString::number(calendarObject->getPriority()) + "\r\n");

    requestString.append("END:VTODO\r\nEND:VCALENDAR");

    connectionToFinish = connect(connectionManager, &ConnectionManager::onFinished, this,
                                 &CalendarObjectWidget::manageResponse);
    connectionManager->addOrUpdateCalendarObject(requestString, calendarObject->getUID());
}
