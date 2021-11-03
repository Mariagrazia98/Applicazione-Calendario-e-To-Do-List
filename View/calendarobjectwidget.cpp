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
//    setupUI();
}

CalendarObjectWidget::~CalendarObjectWidget() {
    delete ui;
}


void CalendarObjectWidget::setupUI() {
    auto calendarObject_ = calendarObject.lock();
    std::shared_ptr<CalendarEvent> calendarEvent = std::shared_ptr<CalendarEvent>(dynamic_cast<CalendarEvent*>(calendarObject_.get()));
    checkBox->adjustSize();
    if (calendarEvent.get()!=nullptr) {
        checkBox->setVisible(false);
        displayLayout->addSpacing(17);
    } else {
        std::shared_ptr <CalendarToDo> calendarToDO = std::shared_ptr<CalendarToDo>(dynamic_cast<CalendarToDo *>(calendarObject_.get()));
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
    auto calendarObject_ = calendarObject.lock();
    QString text;
    text.append("Name: " + calendarObject_->getName() + '\n');
    if (!calendarObject_->getLocation().isEmpty()) {
        text.append("Location: " + calendarObject_->getLocation() + '\n');
    }
    if (!calendarObject_->getDescription().isEmpty()) {
        text.append("Description: " + calendarObject_->getDescription() + '\n');
    }
    text.append(
            "Start date and time: " + calendarObject_->getStartDateTime().toString("dddd, yyyy/MM/d hh:mm") + '\n');
    std::shared_ptr <CalendarEvent> calendarEvent = std::shared_ptr<CalendarEvent> (dynamic_cast<CalendarEvent *>(calendarObject_.get()));
    if (calendarEvent.get() != nullptr) {
        // calendarObject is a CalendarEvent

        text.append("End date and time: " + calendarEvent->getEndDateTime().toString("dddd, yyyy/MM/d hh:mm") + '\n');
    } else {
        std::shared_ptr <CalendarToDo> calendarToDo = std::shared_ptr<CalendarToDo> (dynamic_cast<CalendarToDo *>(calendarObject_.get()));
        if (calendarToDo.get() != nullptr) {
            // calendarObject is a CalendarEvent
            if (calendarToDo->getCompletedDateTime()) {
                textBrowser->setTextColor(QColor(0, 150, 0));
            }
        }
    }
    text.append("CalendarName: " + calendarObject_->getCalendarName() + '\n');
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
    auto calendarObject_ = calendarObject.lock();
    TaskForm *taskForm = new TaskForm(connectionManagers, calendarObject_.get());
    taskForm->show();
    connectionToObjectModified = connect(taskForm, &TaskForm::taskUploaded, this,
                                         &CalendarObjectWidget::onTaskModified);
}

void CalendarObjectWidget::onRemoveButtonClicked() {
    auto calendarObject_ = calendarObject.lock();
    if (calendarObject_->getTypeRepetition() > 0 && calendarObject_->getNumRepetition() != 0) {
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
    auto calendarObject_ = calendarObject.lock();
    if (type == 0) { // delete all recurrences
        deleteCalendarObject();
    } else if (type == 1) { // delete only one recurrence

        QString objectType;
        std::shared_ptr <CalendarToDo> calendarToDo = std::shared_ptr <CalendarToDo> (dynamic_cast<CalendarToDo *>(calendarObject_.get()));
        if (calendarToDo.get()) {
            objectType = "VTODO";
        } else {
            objectType = "VEVENT";
        }

        QString requestString =
                "BEGIN:VCALENDAR\r\nBEGIN:" + objectType + "\r\nUID:" + calendarObject_->getUID() +
                "\r\nVERSION:2.0\r\nDTSTAMP:" +
                calendarObject_->getCreationDateTime().toString("yyyyMMddTHHmmssZ") +
                "\r\nSUMMARY:" + calendarObject_->getName() + "\r\nLOCATION:" +
                calendarObject_->getLocation() + "\r\nDESCRIPTION:" + calendarObject_->getDescription() +
                "\r\nTRANSP:OPAQUE\r\n";
        if (auto parent = calendarObject_->getParent().lock()) {
            requestString.append(
                    "DTSTART:" + (*parent)->getStartDateTime().toString("yyyyMMddTHHmmssZ") +
                    "\r\n");
        } else {
            requestString.append("DTSTART:" + calendarObject_->getStartDateTime().toString("yyyyMMddTHHmmssZ") + "\r\n");
        }

        if (calendarToDo) {
            if (calendarToDo->getCompletedDateTime()) {
                requestString.append(
                        "COMPLETED:" + calendarToDo->getCompletedDateTime()->toString("yyyyMMddTHHmmssZ") + "\r\n");
                requestString.append("STATUS:COMPLETED\r\n");
            } else {
                requestString.append("STATUS:IN-PROCESS\r\n");
            }
        } else {
            std::shared_ptr <CalendarEvent> calendarEvent = std::shared_ptr <CalendarEvent> ( dynamic_cast<CalendarEvent *>(calendarObject_.get()));
            if (auto parent = calendarEvent->getParent().lock()){
                const CalendarEvent *parent_event = dynamic_cast<const CalendarEvent *>(*parent);
                requestString.append(
                        "DTEND:" + parent_event->getEndDateTime().toString("yyyyMMddTHHmmssZ") + "\r\n");
            } else {
                requestString.append("DTEND:" + calendarEvent->getEndDateTime().toString("yyyyMMddTHHmmssZ") + "\r\n");
            }

        }

        QDate recurrenceDate;
        recurrenceDate = calendarObject_->getStartDateTime().date();

        QList<QDate> exDates = calendarObject_->getExDates();
        requestString.append("EXDATE:");
        for (int i = 0; i < exDates.size(); i++) {
            requestString.append(exDates[i].toString("yyyyMMddT010000Z") + ",");
        }
        //std::cout << "deleting reccurence on " + recurrenceDate.toString("yyyyMMddT010000Z").toStdString() + '\n';
        requestString.append(recurrenceDate.toString("yyyyMMddT010000Z") + "\r\n");

        if (calendarObject_->getTypeRepetition() > 0 && calendarObject_->getNumRepetition() != 0) {
            QString rrule = "RRULE:FREQ=";
            switch (calendarObject_->getTypeRepetition()) {
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
            rrule += ";COUNT=" + QString::number(calendarObject_->getNumRepetition()) + "\r\n";
            requestString.append(rrule);
        }


        requestString.append("PRIORITY:" + QString::number(calendarObject_->getPriority()) + "\r\n");

        requestString.append("UNTIL:" + calendarObject_->getUntilDateRipetition().toString("yyyyMMdd") +
                             "\r\n");


        requestString.append("END:" + objectType + "\r\nEND:VCALENDAR");

        //std::cout << requestString.toStdString() << "\n";

        std::shared_ptr<ConnectionManager> connectionManager = connectionManagers[calendarObject_->getCalendarName()];

        connectionToFinish = connect(connectionManager.get(), SIGNAL(insertOrUpdatedCalendarObject(QNetworkReply * )),
                                     this,
                                     SLOT(manageResponse(QNetworkReply * )));
        connectionManager->addOrUpdateCalendarObject(requestString, calendarObject_->getUID());
    }
}

void CalendarObjectWidget::deleteCalendarObject() {
    auto calendarObject_ = calendarObject.lock();
    std::shared_ptr<ConnectionManager> connectionManager = connectionManagers[calendarObject_->getCalendarName()];
    connectionToFinish = connect(connectionManager.get(), SIGNAL(objectDeleted(QNetworkReply * )), this,
                                 SLOT(manageResponse(QNetworkReply * )));
    connectionManager->deleteCalendarObject(calendarObject_->getUID());
}

void CalendarObjectWidget::manageResponse(QNetworkReply *reply) {
    auto calendarObject_ = calendarObject.lock();
    disconnect(connectionToFinish);
    if (reply != nullptr) {

        QNetworkReply::NetworkError error = reply->error();
        if (error == QNetworkReply::NoError) {
            emit(taskDeleted(*calendarObject_));
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
    auto calendarObject_ = calendarObject.lock();
    disconnect(connectionToObjectModified);
    emit(taskModified(calendarObject_->getCalendarName()));
}

void CalendarObjectWidget::onCheckBoxToggled(bool checked) {
    auto calendarObject_ = calendarObject.lock();
    //std::cout << "on check box toggled onCheckBoxToggledn";
    std::shared_ptr <CalendarToDo> calendarToDo = std::shared_ptr<CalendarToDo> (dynamic_cast<CalendarToDo *>(calendarObject_.get()));
    if (checked) {
        calendarToDo->setCompletedDateTime(QDateTime::currentDateTime());
    } else {
        calendarToDo->getCompletedDateTime().reset();
    }

    QString requestString = "BEGIN:VCALENDAR\r\n"
                            "BEGIN:VTODO\r\n"
                            "UID:" + calendarObject_->getUID() + "\r\n"
                                                                "VERSION:2.0\r\n"
                                                                "DTSTAMP:" +
                            calendarObject_->getCreationDateTime().toString("yyyyMMddTHHmmssZ") +
                            "\r\n"
                            "SUMMARY:" + calendarObject_->getName() + "\r\n""LOCATION:" +
                            calendarObject_->getLocation() + "\r\n""DESCRIPTION:" + calendarObject_->getDescription() +
                            "\r\n""TRANSP:OPAQUE\r\n";

    if (auto parent = calendarObject_->getParent().lock()){
        requestString.append(
                "DTSTART:" + (*parent)->getStartDateTime().toString("yyyyMMddTHHmmssZ") +
                "\r\n");
    } else {
        requestString.append("DTSTART:" + calendarObject_->getStartDateTime().toString("yyyyMMddTHHmmssZ") + "\r\n");
    }


    requestString.append("UNTIL:" + calendarObject_->getUntilDateRipetition().toString("yyyyMMddT000000Z") + "\r\n");

    if (calendarToDo->getCompletedDateTime()) {
        //std::cout << "GETCOMPLETE\n";
        requestString.append(
                "COMPLETED:" + calendarToDo->getCompletedDateTime()->toString("yyyyMMddTHHmmssZ") + "\r\n");
        requestString.append("STATUS:COMPLETED\r\n");
    } else {
        requestString.append("STATUS:IN-PROCESS\r\n");
    }

    if (calendarObject_->getTypeRepetition() > 0 && calendarObject_->getNumRepetition() != 0) {
        QString rrule = "RRULE:FREQ=";
        switch (calendarObject_->getTypeRepetition()) {
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
        rrule += ";COUNT=" + QString::number(calendarObject_->getNumRepetition()) + "\r\n";
        requestString.append(rrule);
    }

    requestString.append("PRIORITY:" + QString::number(calendarObject_->getPriority()) + "\r\n");

    requestString.append("END:VTODO\r\nEND:VCALENDAR");

    //std::cout << requestString.toStdString() << "\n";

    std::shared_ptr<ConnectionManager> connectionManager = connectionManagers[calendarObject_->getCalendarName()];

    connectionToFinish = connect(connectionManager.get(), &ConnectionManager::onFinished, this,
                                 &CalendarObjectWidget::manageResponse);
    connectionManager->addOrUpdateCalendarObject(requestString, calendarObject_->getUID());
}
