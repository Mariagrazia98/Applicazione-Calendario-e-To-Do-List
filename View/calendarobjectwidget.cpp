#include "calendarobjectwidget.h"
#include "ui_calendarobjectwidget.h"
#include "eliminationtaskdialog.h"

#include <iostream>
#include <utility>

CalendarObjectWidget::CalendarObjectWidget(QWidget *parent, const std::shared_ptr<CalendarObject> &calendarObject,
                                           QMap<QString, std::shared_ptr<ConnectionManager>> connectionManagers) :
        QWidget(parent),
        calendarObject(calendarObject),
        displayLayout(new QHBoxLayout),
        buttonsLayout(new QVBoxLayout),
        checkBox(new QCheckBox),
        textBrowser(new QTextBrowser),
        modifyButton(new QPushButton(this)),
        removeButton(new QPushButton(this)),
        connectionManagers(std::move(connectionManagers)),
        ui(new Ui::CalendarObjectWidget) {

    ui->setupUi(this);
    if (!calendarObject.get()) {
        /* It hasn't received any calendarObject */
        std::cerr << "calendar object null\n";
    }
}

CalendarObjectWidget::~CalendarObjectWidget() {
    delete ui;
}

void CalendarObjectWidget::setupUI() {
    /* Setup general UI */
    std::shared_ptr<CalendarEvent> calendarEvent = std::dynamic_pointer_cast<CalendarEvent>(calendarObject);
    checkBox->adjustSize();
    if (calendarEvent.get() != nullptr) {
        /* CalendarObject this widget displays is an event */
        checkBox->setVisible(false);
        displayLayout->addSpacing(17);
    } else {
        /* CalendarObject this widget displays is a task */
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

    /* Calls other sets up */
    setupText();
    displayLayout->addWidget(textBrowser);
    setupButtons();
    this->setLayout(displayLayout);
}

void CalendarObjectWidget::setupText() {
    /* Set calendarObject information to be displayed  */
    QString text;
    text.append("Name: " + calendarObject->getName() + '\n');
    if (!calendarObject->getLocation().isEmpty()) {
        text.append("Location: " + calendarObject->getLocation() + '\n');
    }
    if (!calendarObject->getDescription().isEmpty()) {
        text.append("Description: " + calendarObject->getDescription() + '\n');
    }
    text.append("Start date and time: " + calendarObject->getStartDateTime().toString("dddd, yyyy/MM/d hh:mm") + '\n');
    std::shared_ptr<CalendarEvent> calendarEvent = std::dynamic_pointer_cast<CalendarEvent>(calendarObject);
    if (calendarEvent.get() != nullptr) {
        /* calendarObject is a CalendarEvent */
        text.append("End date and time: " + calendarEvent->getEndDateTime().toString("dddd, yyyy/MM/d hh:mm") + '\n');
    } else {
        std::shared_ptr<CalendarToDo> calendarToDo = std::dynamic_pointer_cast<CalendarToDo>(calendarObject);
        if (calendarToDo.get() != nullptr) {
            /* calendarObject is a CalendarEvent */
            if (calendarToDo->getCompletedDate().contains(calendarToDo->getStartDateTime().date())) {
                textBrowser->setTextColor(QColor(0, 150, 0));
            }
        }
    }
    text.append("Name of the calendar: " + calendarObject->getCalendarName() + '\n');
    textBrowser->setText(text);
}

void CalendarObjectWidget::setupButtons() {
    /* Set buttons to be displayed  */
    modifyButton->setIcon(QIcon(":/resources/edit.png"));
    if (modifyButton->icon().isNull()) {
        modifyButton->setText("Modify");
    }
    modifyButton->setToolTip("Modify");
    buttonsLayout->addWidget(modifyButton);
    removeButton->setIcon(QIcon(":/resources/garbage.png"));
    if (removeButton->icon().isNull()) {
        removeButton->setText(QStringLiteral(u"Remove"));
    }
    removeButton->setToolTip(QStringLiteral(u"Remove"));
    buttonsLayout->addWidget(removeButton);
    displayLayout->addLayout(buttonsLayout);

    /* Set connections between buttons and their event listener */
    connect(modifyButton, &QPushButton::clicked, this, &CalendarObjectWidget::onModifyButtonClicked);
    connect(removeButton, &QPushButton::clicked, this, &CalendarObjectWidget::onRemoveButtonClicked);
}

void CalendarObjectWidget::onModifyButtonClicked() {
    /* When modify button is clicked a pre-compiled CalendarObjectForm is displayed */
    CalendarObjectForm *taskForm = new CalendarObjectForm(connectionManagers, calendarObject.get());
    taskForm->show();
    connectionToObjectModified = connect(taskForm, &CalendarObjectForm::taskUploaded, this,
                                         &CalendarObjectWidget::onTaskModified);
}

void CalendarObjectWidget::onTaskModified() {
    /* calendarObject successfully modified */
    disconnect(connectionToObjectModified);
    emit(taskModified(calendarObject->getCalendarName()));
}

void CalendarObjectWidget::onRemoveButtonClicked() {
    assert(calendarObject);
    if (calendarObject->getTypeRepetition() > 0 && calendarObject->getNumRepetition() != 0) {
        /* The calendarObject the user wants to modify has repetitions,
         * an EliminationTaskDialog is shown to ask if he wants to delete all the
         * recurrences or just this one */
        EliminationTaskDialog *dialog = new EliminationTaskDialog(nullptr);
        dialog->show();
        connect(dialog, &EliminationTaskDialog::eliminateRecurrences, this,
                &CalendarObjectWidget::handleDeleteRecurrencies);
    } else {
        /* The calendarObject the user wants to modify has no repetitions */
        deleteCalendarObject();
    }
}

void CalendarObjectWidget::handleDeleteRecurrencies(int type) {
    if (type == 0) {
        /* The user chose to delete all recurrences */
        deleteCalendarObject();
    } else if (type == 1) {
        /* The user chose to delete only one recurrence */
        bool isEvent = false;
        calendarObject->addExDate(calendarObject->getStartDateTime().date());

        std::shared_ptr<CalendarEvent> calendarEvent = std::dynamic_pointer_cast<CalendarEvent>(calendarObject);
        if (calendarEvent.get() != nullptr) {
            isEvent = true;
        }

        if (auto parent = calendarObject->getParent().lock()) {
            parent->addExDate(calendarObject->getStartDateTime().date());
        } else {
            QDateTime newStartDateTime = calendarObject->getStartDateTime();
            QDateTime newEndDateTime;
            if (isEvent) {
                newEndDateTime = calendarEvent->getEndDateTime();
            } else {
                newEndDateTime = newStartDateTime;
            }

            while (calendarObject->getExDates().contains(newStartDateTime.date()) &&
                   newStartDateTime.date() < calendarObject->getUntilDateRepetition()) {

                /* if the event takes more than one day i have to readjust the dates */

                if (newStartDateTime.date() != newEndDateTime.date()) {
                    qint64 diff = newStartDateTime.daysTo(newEndDateTime);
                    newStartDateTime = newStartDateTime.addDays(diff);
                    newEndDateTime = newEndDateTime.addDays(diff);
                }

                switch (calendarObject->getTypeRepetition()) {
                    case CalendarObject::RepetitionType::DAILY:
                        newStartDateTime = newStartDateTime.addDays(calendarObject->getNumRepetition());
                        newEndDateTime = newEndDateTime.addDays(calendarObject->getNumRepetition());
                        break;
                    case CalendarObject::RepetitionType::WEEKLY:
                        newStartDateTime = newStartDateTime.addDays(calendarObject->getNumRepetition() * 7);
                        newEndDateTime = newEndDateTime.addDays(calendarObject->getNumRepetition() * 7);
                        break;
                    case CalendarObject::RepetitionType::MONTHLY:
                        newStartDateTime = newStartDateTime.addMonths(calendarObject->getNumRepetition());
                        newEndDateTime = newEndDateTime.addMonths(calendarObject->getNumRepetition());
                        break;
                    case CalendarObject::RepetitionType::YEARLY:
                        newStartDateTime = newStartDateTime.addYears(calendarObject->getNumRepetition());
                        newEndDateTime = newEndDateTime.addYears(calendarObject->getNumRepetition());
                        break;
                    default:
                        break;
                }
            }
            if (newStartDateTime.date() >= calendarObject->getUntilDateRepetition()) {
                // this is the last one recurrence remaining
                deleteCalendarObject();
                return;
            }

            calendarObject->setStartDateTime(newStartDateTime);
            if (isEvent) {
                calendarEvent->setEndDateTime(newEndDateTime);
            }
        }
        updateObject(isEvent);
    }
}

void CalendarObjectWidget::deleteCalendarObject() {
    /* The user chose to delete all recurrences or calendarObject has no recurrences */
    /* Selection of the connection manager which will manage the delete request */
    std::shared_ptr<ConnectionManager> connectionManager = connectionManagers[calendarObject->getCalendarName()];
    connectionToFinish = connect(connectionManager.get(), SIGNAL(objectDeleted(QNetworkReply * )), this,
                                 SLOT(manageResponse(QNetworkReply * )));
    connectionManager->deleteCalendarObject(calendarObject->getUID());
}

void CalendarObjectWidget::manageResponse(QNetworkReply *reply) {
    /* Handling response of the delete/update request */
    disconnect(connectionToFinish);
    if (reply != nullptr) {
        QNetworkReply::NetworkError error = reply->error();
        if (error == QNetworkReply::NoError) {
            /* Success */
            emit(taskDeleted(*calendarObject));
        } else {
            /* Error */
            const QString &errorString = reply->errorString();
            QByteArray answer = reply->readAll();
            QString answerString = QString::fromUtf8(answer);
            QMessageBox::warning(this, "Error", errorString);
        }
    } else {
        /* Null reply */
        QMessageBox::warning(this, "Error", "Something went wrong");
    }
}

void CalendarObjectWidget::onCheckBoxToggled(bool checked) {
    /* the user wants to change the status of the calendarObject (completed or not) */
    auto calendarToDo = std::dynamic_pointer_cast<CalendarToDo>(calendarObject);
    if (checked) {
        /* calendarObject will be marked as completed */
        calendarToDo->addCompletedDate(calendarToDo->getStartDateTime().date());
        // if this a child objecr
        if (auto parent = calendarObject->getParent().lock()) {
            auto parentToDo = std::dynamic_pointer_cast<CalendarToDo>(parent);
            // add to completedDates
            parentToDo->addCompletedDate(calendarToDo->getStartDateTime().date());
        }
    } else {
        /* calendarObject will be marked as not completed */
        calendarToDo->removeCompletedDate(calendarToDo->getStartDateTime().date());
        if (auto parent = calendarObject->getParent().lock()) {
            auto parentToDo = std::dynamic_pointer_cast<CalendarToDo>(parent);
            parentToDo->removeCompletedDate(calendarToDo->getStartDateTime().date());
        }
    }

    updateObject(false);
}

void CalendarObjectWidget::updateObject(bool isEvent) {
    QString objectType;
    if (isEvent) {
        objectType = "VEVENT";
    } else {
        objectType = "VTODO";
    }

    /* Composition of the request */
    QString requestString =
            "BEGIN:VCALENDAR\r\nBEGIN:" + objectType + "\r\nUID:" + calendarObject->getUID() +
            "\r\nVERSION:2.0\r\nDTSTAMP:" + calendarObject->getCreationDateTime().toString("yyyyMMddTHHmmssZ") +
            "\r\nSUMMARY:" + calendarObject->getName();
    if (!calendarObject->getLocation().isEmpty()) {
        requestString.append("\r\nLOCATION:" + calendarObject->getLocation());
    }
    if (!calendarObject->getDescription().isEmpty()) {
        requestString.append("\r\nDESCRIPTION:" + calendarObject->getDescription());
    }
    requestString.append("\r\nTRANSP:OPAQUE\r\n");
    if (auto parent = calendarObject->getParent().lock()) {
        requestString.append("DTSTART:" + parent->getStartDateTime().toString("yyyyMMddTHHmmssZ") + "\r\n");
    } else {
        requestString.append("DTSTART:" + calendarObject->getStartDateTime().toString("yyyyMMddTHHmmssZ") + "\r\n");
    }

    if (isEvent) {
        /* Field of CalendarEvent */
        std::shared_ptr<CalendarEvent> calendarEvent = std::dynamic_pointer_cast<CalendarEvent>(calendarObject);
        if (auto parent = calendarEvent->getParent().lock()) {
            auto parent_event = std::dynamic_pointer_cast<const CalendarEvent>(parent);
            requestString.append(
                    "DTEND:" + parent_event->getEndDateTime().toString("yyyyMMddTHHmmssZ") + "\r\n");
        } else {
            requestString.append("DTEND:" + calendarEvent->getEndDateTime().toString("yyyyMMddTHHmmssZ") + "\r\n");
        }
    } else {
        /* Field of CalendarToDo */
        std::shared_ptr<CalendarToDo> calendarToDo = std::dynamic_pointer_cast<CalendarToDo>(calendarObject);
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
    }
    /* Part of the request related to the repetitions */
    if (calendarObject->getTypeRepetition() != CalendarObject::RepetitionType::NONE &&
        calendarObject->getNumRepetition() > 0) {
        QString rrule = "RRULE:FREQ=";
        switch (calendarObject->getTypeRepetition()) {
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
        rrule += ";COUNT=" + QString::number(calendarObject->getNumRepetition()) + "\r\n";
        requestString.append(rrule);
    }

    if (!calendarObject->getExDates().isEmpty()) {
        /* In order to delete only one recurrence, the date in which this widget is
         * displayed must be added as an EXDATE in the calendarObject */
        QSet<QDate> exDates = calendarObject->getExDates();
        requestString.append("EXDATE:");
        QSet<QDate>::const_iterator i = exDates.constBegin();
        while (i != exDates.constEnd()) {
            requestString.append(i->toString("yyyyMMddT010000Z"));
            ++i;
            if (i != exDates.constEnd()) {
                requestString.append(',');
            }
        }
        requestString.append("\r\n");
    }

    requestString.append("PRIORITY:" + QString::number(calendarObject->getPriority()) + "\r\n");

    requestString.append("UNTIL:" + calendarObject->getUntilDateRepetition().toString("yyyyMMddT010000Z") + "\r\n");

    requestString.append("END:" + objectType + "\r\nEND:VCALENDAR");
    /* Composition of request end */

    /* Passing the requestString to the connectionManager which will handle the request */
    std::shared_ptr<ConnectionManager> connectionManager = connectionManagers[calendarObject->getCalendarName()];

    connectionToFinish = connect(connectionManager.get(), SIGNAL(insertOrUpdatedCalendarObject(QNetworkReply * )),
                                 this, SLOT(manageResponse(QNetworkReply * )));
    connectionManager->addOrUpdateCalendarObject(requestString, calendarObject->getUID());

}