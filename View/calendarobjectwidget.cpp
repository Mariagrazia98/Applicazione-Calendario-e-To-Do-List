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
        connectionManager(std::make_shared<ConnectionManager*>(connectionManager)),
        ui(new Ui::CalendarObjectWidget) {
    ui->setupUi(this);
//    setupUI();
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
    text.append(
            "Start date and time: " + calendarObject->getStartDateTime().toString("dddd, yyyy/MM/d hh:mm") + '\n');
    CalendarEvent *calendarEvent = dynamic_cast<CalendarEvent *>(calendarObject);
    if (calendarEvent != nullptr) {
        // calendarObject is a CalendarEvent

        text.append("End date and time: " + calendarEvent->getEndDateTime().toString("dddd, yyyy/MM/d hh:mm") + '\n');
    } else {
        CalendarToDo *calendarToDo = dynamic_cast<CalendarToDo *>(calendarObject);
        if (calendarToDo != nullptr) {
            // calendarObject is a CalendarEvent
            if (calendarToDo->getCompletedDateTime()) {
                textBrowser->setTextColor(QColor(0, 150, 0));
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
    TaskForm *taskForm = new TaskForm(*connectionManager.get(), calendarObject);
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
        if (calendarObject->getParent().get()) {
            requestString.append(
                    "DTSTART:" + (*calendarObject->getParent().get())->getStartDateTime().toString("yyyyMMddTHHmmssZ") +
                    "\r\n");
        } else {
            requestString.append("DTSTART:" + calendarObject->getStartDateTime().toString("yyyyMMddTHHmmssZ") + "\r\n");
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
            CalendarEvent *calendarEvent = dynamic_cast<CalendarEvent *>(calendarObject);
            if (calendarEvent->getParent().get()) {
                const CalendarEvent *parent = dynamic_cast<const CalendarEvent *>(*calendarEvent->getParent().get());
                requestString.append(
                        "DTEND:" + parent->getEndDateTime().toString("yyyyMMddTHHmmssZ") + "\r\n");
            } else {
                requestString.append("DTEND:" + calendarEvent->getEndDateTime().toString("yyyyMMddTHHmmssZ") + "\r\n");
            }

        }

        QDate recurrenceDate;
        recurrenceDate = calendarObject->getStartDateTime().date();

        QList<QDate> exDates = calendarObject->getExDates();
        requestString.append("EXDATE:");
        for (int i = 0; i < exDates.size(); i++) {
            requestString.append(exDates[i].toString("yyyyMMddT010000Z") + ",");
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

        requestString.append("UNTIL:" + calendarObject->getUntilDateRipetition().toString("yyyyMMdd") +
                             "\r\n");


        requestString.append("END:" + objectType + "\r\nEND:VCALENDAR");

        std::cout << requestString.toStdString() << "\n";

        connectionToFinish = connect(*connectionManager.get(), SIGNAL(insertOrUpdatedCalendarObject(QNetworkReply * )), this,
                                     SLOT(manageResponse(QNetworkReply * )));
        (*connectionManager.get())->addOrUpdateCalendarObject(requestString, calendarObject->getUID());
    }
}

void CalendarObjectWidget::deleteCalendarObject() {
    connectionToFinish = connect(*connectionManager.get(), SIGNAL(objectDeleted(QNetworkReply * )), this,
                                 SLOT(manageResponse(QNetworkReply * )));
    (*connectionManager.get())->deleteCalendarObject(calendarObject->getUID());
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
    emit(taskModified());
}

void CalendarObjectWidget::onCheckBoxToggled(bool checked) {
    std::cout << "on check box toggled\fonCheckBoxToggledn";
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
                            "SUMMARY:" + calendarObject->getName() + "\r\n""LOCATION:" +
                            calendarObject->getLocation() + "\r\n""DESCRIPTION:" + calendarObject->getDescription() +
                            "\r\n""TRANSP:OPAQUE\r\n";
    std::cout << "dtstart\n";
    if (calendarObject->getParent().get()) {
        std::cout << "if\n";
        requestString.append(
                "DTSTART:" + (*calendarObject->getParent().get())->getStartDateTime().toString("yyyyMMddTHHmmssZ") +
                "\r\n");
    } else {
        requestString.append("DTSTART:" + calendarObject->getStartDateTime().toString("yyyyMMddTHHmmssZ") + "\r\n");
    }


    requestString.append("UNTIL:" + calendarObject->getUntilDateRipetition().toString("yyyyMMddTHHmmss") + "\r\n");

    if (calendarToDo->getCompletedDateTime()) {
        std::cout << "GETCOMPLETE\n";
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

    requestString.append("PRIORITY:" + QString::number(calendarObject->getPriority()) + "\r\n");

    std::cout << requestString.toStdString() << "\n";

    connectionToFinish = connect(*connectionManager.get(), &ConnectionManager::onFinished, this,
                                 &CalendarObjectWidget::manageResponse);
    (*connectionManager.get())->addOrUpdateCalendarObject(requestString, calendarObject->getUID());
}
