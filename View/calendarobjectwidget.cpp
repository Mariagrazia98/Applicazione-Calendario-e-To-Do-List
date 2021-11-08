#include "calendarobjectwidget.h"
#include "ui_calendarobjectwidget.h"
#include "eliminationtaskdialog.h"

#include <iostream>

#define DAILY 1
#define WEEKLY 2
#define MONTHLY 3
#define YEARLY 4

CalendarObjectWidget::CalendarObjectWidget(QWidget *parent, std::shared_ptr<CalendarObject> calendarObject,
                                           QMap<QString, std::shared_ptr<ConnectionManager>> connectionManagers) :
        QWidget(parent),
        calendarObject(calendarObject),
        displayLayout(new QHBoxLayout),
        buttonsLayout(new QVBoxLayout),
        checkBox(new QCheckBox),
        textBrowser(new QTextBrowser),
        modifyButton(new QPushButton(this)),
        removeButton(new QPushButton(this)),
        connectionManagers(connectionManagers),
        ui(new Ui::CalendarObjectWidget) {
    ui->setupUi(this);
    if (!calendarObject.get()) {
        std::cout << "calendar object null\n";
    }
//    setupUI();
}

CalendarObjectWidget::~CalendarObjectWidget() {
    delete ui;
}


void CalendarObjectWidget::setupUI() {
    std::shared_ptr<CalendarEvent> calendarEvent = std::dynamic_pointer_cast<CalendarEvent>(calendarObject);
    checkBox->adjustSize();
    if (calendarEvent.get() != nullptr) {
        checkBox->setVisible(false);
        displayLayout->addSpacing(17);
    } else {
        auto calendarToDO = std::dynamic_pointer_cast<CalendarToDo>(calendarObject);
        QList<QDate> completedDate = calendarToDO->getCompletedDate();
        if (completedDate.contains(calendarToDO->getStartDateTime().date())) {
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
    text.append(
            "Start date and time: " + calendarObject->getStartDateTime().toString("dddd, yyyy/MM/d hh:mm") + '\n');
    std::shared_ptr<CalendarEvent> calendarEvent = std::dynamic_pointer_cast<CalendarEvent>(calendarObject);
    if (calendarEvent.get() != nullptr) {
        // calendarObject is a CalendarEvent

        text.append("End date and time: " + calendarEvent->getEndDateTime().toString("dddd, yyyy/MM/d hh:mm") + '\n');
    } else {
        std::shared_ptr<CalendarToDo> calendarToDo = std::dynamic_pointer_cast<CalendarToDo>(calendarObject);
        if (calendarToDo.get() != nullptr) {
            // calendarObject is a CalendarEvent
            if (calendarToDo->getCompletedDate().contains(calendarToDo->getStartDateTime().date())) {
                textBrowser->setTextColor(QColor(0, 150, 0));
            }
        }
    }
    text.append("CalendarName: " + calendarObject->getCalendarName() + '\n');
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
    TaskForm *taskForm = new TaskForm(connectionManagers, calendarObject.get());
    taskForm->show();
    connectionToObjectModified = connect(taskForm, &TaskForm::taskUploaded, this,
                                         &CalendarObjectWidget::onTaskModified);
}

void CalendarObjectWidget::onRemoveButtonClicked() {
    assert(calendarObject);
    if (calendarObject->getTypeRepetition() > 0 && calendarObject->getNumRepetition() != 0) {
        // has repetitions
        EliminationTaskDialog *dialog = new EliminationTaskDialog(nullptr);
        dialog->show();
        connect(dialog, &EliminationTaskDialog::eliminateRecurrences, this,
                &CalendarObjectWidget::handleDeleteRecurrencies);
    } else {
        deleteCalendarObject();
    }
}

void CalendarObjectWidget::handleDeleteRecurrencies(int type) {
    if (type == 0) { // delete all recurrences
        deleteCalendarObject();
    } else if (type == 1) { // delete only one recurrence
        calendarObject->addExDate(calendarObject->getStartDateTime().date());
        if (auto parent = calendarObject->getParent().lock()) {
            parent->addExDate(calendarObject->getStartDateTime().date());
        }
        QString objectType;
        std::shared_ptr<CalendarToDo> calendarToDo = std::dynamic_pointer_cast<CalendarToDo>(calendarObject);
        if (calendarToDo.get()) {
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
        if (auto parent = calendarObject->getParent().lock()) {
            requestString.append(
                    "DTSTART:" + parent->getStartDateTime().toString("yyyyMMddTHHmmssZ") +
                    "\r\n");
        } else {
            requestString.append(
                    "DTSTART:" + calendarObject->getStartDateTime().toString("yyyyMMddTHHmmssZ") + "\r\n");
        }

        if (calendarToDo) {
            if (!calendarToDo->getCompletedDate().isEmpty()) {
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
        } else {
            std::shared_ptr<CalendarEvent> calendarEvent = std::dynamic_pointer_cast<CalendarEvent>(calendarObject);
            if (auto parent = calendarEvent->getParent().lock()) {
                auto parent_event = std::dynamic_pointer_cast<const CalendarEvent>(parent);
                requestString.append(
                        "DTEND:" + parent_event->getEndDateTime().toString("yyyyMMddTHHmmssZ") + "\r\n");
            } else {
                requestString.append("DTEND:" + calendarEvent->getEndDateTime().toString("yyyyMMddTHHmmssZ") + "\r\n");
            }

        }

        QDate recurrenceDate;
        recurrenceDate = calendarObject->getStartDateTime().date();

        QSet<QDate> exDates = calendarObject->getExDates();
        requestString.append("EXDATE:");
        QSet<QDate>::const_iterator i = exDates.constBegin();
        while (i != exDates.constEnd()) {
            requestString.append(i->toString("yyyyMMddT010000Z")+',');
            i++;
        }
        //std::cout << "deleting reccurence on " + recurrenceDate.toString("yyyyMMddT010000Z").toStdString() + '\n';
        requestString.append(recurrenceDate.toString("yyyyMMddT010000Z") + "\r\n");

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

        requestString.append("UNTIL:" + calendarObject->getUntilDateRepetition().toString("yyyyMMdd") +
                             "\r\n");


        requestString.append("END:" + objectType + "\r\nEND:VCALENDAR");

        std::shared_ptr<ConnectionManager> connectionManager = connectionManagers[calendarObject->getCalendarName()];

        connectionToFinish = connect(connectionManager.get(), SIGNAL(insertOrUpdatedCalendarObject(QNetworkReply * )),
                                     this,
                                     SLOT(manageResponse(QNetworkReply * )));
        connectionManager->addOrUpdateCalendarObject(requestString, calendarObject->getUID());
    }
}

void CalendarObjectWidget::deleteCalendarObject() {
    std::shared_ptr<ConnectionManager> connectionManager = connectionManagers[calendarObject->getCalendarName()];
    connectionToFinish = connect(connectionManager.get(), SIGNAL(objectDeleted(QNetworkReply * )), this,
                                 SLOT(manageResponse(QNetworkReply * )));
    connectionManager->deleteCalendarObject(calendarObject->getUID());
}

void CalendarObjectWidget::manageResponse(QNetworkReply *reply) {
    disconnect(connectionToFinish);
    if (reply != nullptr) {
        QNetworkReply::NetworkError error = reply->error();
        if (error == QNetworkReply::NoError) {
            emit(taskDeleted(*calendarObject));
        } else {
            const QString &errorString = reply->errorString();
            QByteArray answer = reply->readAll();
            QString answerString = QString::fromUtf8(answer);
            std::cerr << answerString.toStdString() << "\n";
            QMessageBox::warning(this, "Error", errorString);
        }
    } else {
        // reply is null
        QMessageBox::warning(this, "Error", "Something went wrong");
    }

}

void CalendarObjectWidget::onTaskModified() {
    disconnect(connectionToObjectModified);
    emit(taskModified(calendarObject->getCalendarName()));
}

void CalendarObjectWidget::onCheckBoxToggled(bool checked) {
    //std::cout << "on check box toggled onCheckBoxToggledn";
    auto calendarToDo = std::dynamic_pointer_cast<CalendarToDo>(calendarObject);
    if (checked) {
        calendarToDo->addCompletedDate(calendarToDo->getStartDateTime().date());
        if (auto parent = calendarObject->getParent().lock()) {
            auto parentToDo = std::dynamic_pointer_cast<CalendarToDo>(parent);
            parentToDo->addCompletedDate(calendarToDo->getStartDateTime().date());
        }
    } else {
        calendarToDo->removeCompletedDate(calendarToDo->getStartDateTime().date());
        if (auto parent = calendarObject->getParent().lock()) {
            auto parentToDo = std::dynamic_pointer_cast<CalendarToDo>(parent);
            parentToDo->removeCompletedDate(calendarToDo->getStartDateTime().date());
        }
    }

    QString requestString = "BEGIN:VCALENDAR\r\n"
                            "BEGIN:VTODO\r\n"
                            "UID:" + calendarObject->getUID() + "\r\n"
                                                                "VERSION:2.0\r\n"
                                                                "DTSTAMP:" +
                            calendarObject->getCreationDateTime().toString("yyyyMMddTHHmmssZ") +
                            "\r\n"
                            "SUMMARY:" + calendarObject->getName() + "\r\n""LOCATION:" +
                            calendarObject->getLocation() + "\r\n""DESCRIPTION:" + calendarObject->getDescription() +
                            "\r\n""TRANSP:OPAQUE\r\n";

    if (auto parent = calendarObject->getParent().lock()) {
        // è un'occorrenza
        // devo salvare la data di inizio del padre
        requestString.append(
                "DTSTART:" + parent->getStartDateTime().toString("yyyyMMddTHHmmssZ") +
                "\r\n");
    } else {
        // non è un'occorrenza
        requestString.append("DTSTART:" + calendarObject->getStartDateTime().toString("yyyyMMddTHHmmssZ") + "\r\n");
    }

    requestString.append("UNTIL:" + calendarObject->getUntilDateRepetition().toString("yyyyMMddT000000Z") + "\r\n");

    // insert exDates
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

    if (!calendarToDo->getCompletedDate().isEmpty()) {
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

    //}

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

    requestString.append("END:VTODO\r\nEND:VCALENDAR");

    //std::cout << requestString.toStdString()<<std::endl;

    //std::cout << requestString.toStdString() << "\n";

    std::shared_ptr<ConnectionManager> connectionManager = connectionManagers[calendarObject->getCalendarName()];

    connectionToFinish = connect(connectionManager.get(), &ConnectionManager::onFinished, this,
                                 &CalendarObjectWidget::manageResponse);
    connectionManager->addOrUpdateCalendarObject(requestString, calendarObject->getUID());
}
