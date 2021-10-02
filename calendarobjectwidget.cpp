#include "calendarobjectwidget.h"
#include "ui_calendarobjectwidget.h"

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
    //checkBox->setMinimumSize(50,50);
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
        connect(checkBox, &QCheckBox::toggled, this, &CalendarObjectWidget::onCheckBoxToggled);
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
    //textBrowser->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
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
    connectionToFinish = connect(connectionManager, SIGNAL(objectDeleted(QNetworkReply * )), this,
                                 SLOT(finished(QNetworkReply * )));
    connectionManager->deleteCalendarObject(calendarObject->getUID());
}

void CalendarObjectWidget::finished(QNetworkReply *reply) {
    disconnect(connectionToFinish);
    QByteArray answer = reply->readAll();
    QString answerString = QString::fromUtf8(answer);
    QNetworkReply::NetworkError error = reply->error();
    const QString &errorString = reply->errorString();
    if (error != QNetworkReply::NoError) {
        std::cerr << error << "\n";
        QMessageBox::warning(this, "Error", errorString);
    } else {
        emit(taskDeleted(*calendarObject));
    }
}

void CalendarObjectWidget::onTaskModified() {
    std::cout << "Task Modified\n";
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
                                 &CalendarObjectWidget::finished);
    connectionManager->addOrUpdateCalendarObject(requestString, calendarObject->getUID());
}
