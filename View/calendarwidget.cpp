#include "calendarwidget.h"

#include <memory>

std::shared_ptr<CalendarObject> parseCalendarObject_parallel(const QString &calendarObjectString);

CalendarWidget::CalendarWidget(QWidget *parent) :
        QMainWindow(parent),
        dateString(new QTextBrowser),
        stream(new QTextStream()),
        shareCalendarButton(new QPushButton("Share Calendar")),
        timerInterval(10000) {


    createCalendarGroupBox();
    setupCalendar();

    setCentralWidget(calendarGroupBox);
    calendarGroupBox->setMinimumSize(calendar->sizeHint());
    tasksGroupBox->setMinimumWidth(calendar->sizeHint().width() * 1.5);

    QDockWidget *dockWidget = new QDockWidget(tr(""), this);
    dockWidget->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    dockWidget->setWidget(tasksGroupBox);
    dockWidget->setFeatures(dockWidget->features() & ~QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::RightDockWidgetArea, dockWidget);

    setMinimumHeight(480);
    setWindowTitle(tr("Calendar Application"));

}

void CalendarWidget::setupCalendar() {
    // CalendarWidget setup
    calendar->setFirstDayOfWeek(Qt::DayOfWeek(1));
    calendar->setMinimumDate(QDate(2000, 1, 1));
    calendar->setMaximumDate(QDate(2121, 31, 12));
    currentDateEdit = new QDateEdit;
    currentDateEdit->setDisplayFormat("dddd, yyyy/MM/d");
    currentDateEdit->setDate(QDate::currentDate());
    currentDateEdit->setDateRange(calendar->minimumDate(),
                                  calendar->maximumDate());


    connect(calendar, &QCalendarWidget::selectionChanged,
            this, &CalendarWidget::selectedDateChanged);
    tasksGroupBox = new QGroupBox(tr("Tasks"));

    tasksLayout = new QVBoxLayout;

    QDate date = currentDateEdit->date();
    QString englishDate = date.toString("dddd, yyyy/MM/d");

    dateString->setText(englishDate);
    tasksLayout->addWidget(dateString);

    dateString->setFixedHeight(30);
    dateString->setAlignment(Qt::AlignCenter);

    tasksGroupBox->setLayout(tasksLayout);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    taskScrollWidget = new QWidget(scrollArea);
    taskViewLayout = new QVBoxLayout();
    taskScrollWidget->setLayout(taskViewLayout);
    scrollArea->setWidget(taskScrollWidget);

    tasksLayout->addWidget(scrollArea);

    addCalendarObjectButton = new QPushButton(tr("&Add"));
    addCalendarObjectButton->setEnabled(true);
    addCalendarObjectButton->setToolTip(tr("Add a new calendar object"));
    connect(addCalendarObjectButton, &QPushButton::clicked, this, &CalendarWidget::addCalendarObjectButtonClicked);
    tasksLayout->addWidget(addCalendarObjectButton);
}

void CalendarWidget::selectedDateChanged() {
    if (currentDateEdit->date() != calendar->selectedDate()) {
        currentDateEdit->setDate(calendar->selectedDate());
        QDate date = currentDateEdit->date();
        QString englishDate = date.toString("dddd, yyyy/MM/d");
        dateString->setText(englishDate);
        dateString->setAlignment(Qt::AlignCenter);
        showSelectedDateCalendarObjects();
    }
}


void CalendarWidget::createCalendarGroupBox() {
    calendarGroupBox = new QGroupBox(tr("Calendar"));

    calendar = new CustomCalendarWidget();

    calendarLayout = new QGridLayout;
    calendarLayout->addWidget(calendar);

    shareCalendarButton->setEnabled(true);
    shareCalendarButton->setToolTip(tr("Share current calendar"));
    connect(shareCalendarButton, &QPushButton::clicked, this, &CalendarWidget::shareCalendarButtonClicked);
    calendarLayout->addWidget(shareCalendarButton);
    calendarGroupBox->setLayout(calendarLayout);
}

void CalendarWidget::parseCalendar(QString calendarString) {
    stream = new QTextStream(&calendarString, QIODevice::ReadOnly);
    QString line;
    QString calendarName = "";
    QStringList strings;
    QString currentString;
    while (stream->readLineInto(&line)) {
        if (line.contains(QStringLiteral("X-WR-CALNAME"))) {
            // saves calendar name
            const int deliminatorPosition = line.indexOf(QLatin1Char(':'));
            calendarName = line.mid(deliminatorPosition + 1, -1);
            // deletes calendarObjects relative to the changed Calendar
            calendarObjects.erase(std::remove_if(std::execution::par, calendarObjects.begin(), calendarObjects.end(),
                                                 [&calendarName](const auto &calendarObject) {
                                                     return calendarObject->getCalendarName() == calendarName;
                                                 }), calendarObjects.end());
        } else if (line.contains(QStringLiteral("BEGIN:VEVENT")) || line.contains(QStringLiteral("BEGIN:VTODO"))) {
            currentString = line;
        } else {
            currentString.append('\n' + line);
        }
        if (line.contains(QStringLiteral("END:VEVENT")) || line.contains(QStringLiteral("END:VTODO"))) {
            strings.append(currentString);
            currentString = "";
        }
    }

    QFuture<std::shared_ptr<CalendarObject>> future = QtConcurrent::mapped(strings,
                                                                           &parseCalendarObject_parallel);
    if (future.isValid()) {
        QList<std::shared_ptr<CalendarObject>> calendarObjectsObtained = future.results();

        std::for_each(std::execution::par_unseq, calendarObjectsObtained.begin(), calendarObjectsObtained.end(),
                      [calendarName](auto &&calendarObject) {
                          calendarObject->setCalendarName(calendarName);
                      });

        calendarObjects.append(calendarObjectsObtained);

        std::sort(calendarObjects.begin(), calendarObjects.end(),
                  [](const std::shared_ptr<CalendarObject> &a, const std::shared_ptr<CalendarObject> &b) {
                      return a->getPriority() > b->getPriority();
                  });

        stream->seek(0);

        this->calendar->setCalendarObjects(calendarObjects);
        showSelectedDateCalendarObjects();
    }


}

/*
void CalendarWidget::parseCalendar(QString calendarString) {
    stream = new QTextStream(&calendarString, QIODevice::ReadOnly);
    QString line;
    QString calendarName = "";
    while (stream->readLineInto(&line)) {
        if (line.contains("X-WR-CALNAME")) {
            // saves calendar name
            const int deliminatorPosition = line.indexOf(QLatin1Char(':'));
            calendarName = line.mid(deliminatorPosition + 1, -1);
            int i = 0;
            // deletes calendarObjects relative to the changed Calendar
            while (i < calendarObjects.length()) {
                if (calendarObjects[i]->getCalendarName() == calendarName) {
                    calendarObjects.removeAt(i);
                } else {
                    i++;
                }
            }
        }
        if (line.contains("BEGIN:VEVENT")) {
            parseCalendarObject(calendarName, 0);
        } else if (line.contains("BEGIN:VTODO")) {
            parseCalendarObject(calendarName, 1);
        }
    }


    std::sort(calendarObjects.begin(), calendarObjects.end(),
              [](std::shared_ptr<CalendarObject> a, std::shared_ptr<CalendarObject> b) {
                  return a.get()->getPriority() > b.get()->getPriority();
              });

    stream->seek(0);

    this->calendar->setCalendarObjects(calendarObjects);
    showSelectedDateCalendarObjects();

}
*/

void CalendarWidget::showSelectedDateCalendarObjects() {
    QLayoutItem *item;
    while ((item = taskViewLayout->layout()->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    for (auto &calendarObject: calendarObjects) {
        if (calendarObject->getExDates().contains(calendar->selectedDate())) {
            continue;
        }
        std::shared_ptr<CalendarEvent> calendarEvent = std::dynamic_pointer_cast<CalendarEvent>(calendarObject);
        /* this is a Calendar Event */
        if (calendarEvent) {
            if (calendarEvent->getStartDateTime().date() <= calendar->selectedDate() &&
                calendarEvent->getEndDateTime().date() >= calendar->selectedDate()) {
                addCalendarObjectWidget(calendarObject);
            } else if (calendarEvent->getTypeRepetition() != CalendarObject::RepetitionType::NONE &&
                       calendarEvent->getNumRepetition() > 0) {
                QDateTime start = calendarEvent->getStartDateTime();
                QDateTime end = calendarEvent->getEndDateTime();

                while (start.date() < calendar->selectedDate() &&
                       start.date() <= calendarEvent->getUntilDateRepetition()) {

                    /*
                     * if the event takes more than one day I have to readjust the dates
                     */
                    if (start.date() != end.date()) {
                        int diff = start.daysTo(end);
                        start = start.addDays(diff);
                        end = end.addDays(diff);
                    }

                    switch (calendarEvent->getTypeRepetition()) {
                        case CalendarObject::RepetitionType::DAILY: {   //daily
                            start = start.addDays(calendarEvent->getNumRepetition());
                            end = end.addDays(calendarEvent->getNumRepetition());
                            break;
                        }
                        case CalendarObject::RepetitionType::WEEKLY: {
                            start = start.addDays(7 * calendarEvent->getNumRepetition());
                            end = end.addDays(7 * calendarEvent->getNumRepetition());
                            break;
                        }
                        case CalendarObject::RepetitionType::MONTHLY: {
                            start = start.addMonths(calendarEvent->getNumRepetition());
                            end = end.addMonths(calendarEvent->getNumRepetition());
                            break;
                        }
                        case CalendarObject::RepetitionType::YEARLY: {      //yearly
                            start = start.addYears(calendarEvent->getNumRepetition());
                            end = end.addYears(calendarEvent->getNumRepetition());
                            break;
                        }
                    }
                    if (!calendarEvent->getExDates().contains(start.date()) &&
                        start.date() <= calendar->selectedDate() &&
                        end.date() >= calendar->selectedDate() &&
                        start.date() <= calendarEvent->getUntilDateRepetition()) {
                        std::shared_ptr<CalendarEvent> calendarEvent_ = std::make_shared<CalendarEvent>(
                                calendarEvent);
                        calendarEvent_->setStartDateTime(start);
                        calendarEvent_->setEndDateTime(end);
                        addCalendarObjectWidget(calendarEvent_);
                        break;
                    }
                }
            }
        } else {
            std::shared_ptr<CalendarToDo> calendarToDo = std::dynamic_pointer_cast<CalendarToDo>(calendarObject);
            QDateTime start;
            start = calendarToDo->getStartDateTime();
            if (start.date() <= calendar->selectedDate()) {
                if (start.date() == calendar->selectedDate()) {
                    addCalendarObjectWidget(calendarToDo);
                } else if (calendarToDo->getTypeRepetition() != CalendarObject::RepetitionType::NONE &&
                           calendarToDo->getNumRepetition() > 0) {
                    if (calendarToDo->getUntilDateRepetition() >= calendar->selectedDate()) {
                        while (start.date() < calendar->selectedDate() &&
                               start.date() <= calendarToDo->getUntilDateRepetition()) {

                            switch (calendarToDo->getTypeRepetition()) {
                                case CalendarObject::RepetitionType::DAILY: {   // daily
                                    start = start.addDays(calendarToDo->getNumRepetition());
                                    break;
                                }
                                case CalendarObject::RepetitionType::WEEKLY: {      //weekly
                                    start = start.addDays(7 * calendarToDo->getNumRepetition());
                                    break;
                                }
                                case CalendarObject::RepetitionType::MONTHLY: {      //monthly
                                    start = start.addMonths(calendarToDo->getNumRepetition());
                                    break;
                                }
                                case CalendarObject::RepetitionType::YEARLY: {      //yearly
                                    start = start.addYears(calendarToDo->getNumRepetition());
                                    break;
                                }
                            }
                            if (start.date() == calendar->selectedDate() &&
                                start.date() <= calendarToDo->getUntilDateRepetition()) {
                                std::shared_ptr<CalendarToDo> calendarToDo_ = std::make_shared<CalendarToDo>(
                                        calendarToDo);
                                calendarToDo_->setStartDateTime(start);
                                addCalendarObjectWidget(calendarToDo_);
                                break;
                            }
                        }
                    }
                }

            }
        }
    }
}


void CalendarWidget::addCalendarObjectWidget(const std::shared_ptr<CalendarObject> &calendarObject) {
    const QString calendarName = calendarObject->getCalendarName();

    // get connectionManager relative to the calendarObject calendar
    if (!connectionManagers.contains(calendarName)) {
        std::cerr << "Cannot find a connection for calendar " << calendarName.toStdString() << '\n';
        return;
    }
    std::shared_ptr<ConnectionManager> connectionManager = connectionManagers[calendarName];
    CalendarObjectWidget *calendarObjectWidget = new CalendarObjectWidget(this, calendarObject, connectionManagers);
    calendarObjectWidget->setupUI();
    calendarObjectWidget->setVisible(true);
    calendarObjectWidget->setEnabled(true);
    taskViewLayout->addWidget(calendarObjectWidget);
    connect(calendarObjectWidget, &CalendarObjectWidget::taskModified, this,
            &CalendarWidget::onTaskModified);
    connect(calendarObjectWidget, &CalendarObjectWidget::taskDeleted, this,
            &CalendarWidget::onTaskDeleted);
}


void CalendarWidget::addCalendarObjectButtonClicked() {
    addCalendarObjectButton->setEnabled(false);
    CalendarObjectForm *taskForm = new CalendarObjectForm(connectionManagers);
    taskForm->setDate(currentDateEdit->date());
    taskForm->show();
    connect(taskForm, &CalendarObjectForm::taskFormClosed, this, &CalendarWidget::onTaskFormClosed);
    connect(taskForm, &CalendarObjectForm::taskUploaded, this, &CalendarWidget::onTaskModified);
}

void CalendarWidget::addConnectionManager(ConnectionManager *connectionManager) {
    QString calendarName = connectionManager->getCalendarDisplayName();
    if (!connectionManagers.contains(calendarName)) {
        connectionManagers.insert(calendarName, std::shared_ptr<ConnectionManager>(connectionManager));
    }
}


void CalendarWidget::getCalendarRequest(const QString &calendarName) {
    std::shared_ptr<ConnectionManager> connectionManager = connectionManagers[calendarName];
    connectionManager->getCalendarRequest();
}

QDate CalendarWidget::getCurrentDateSelected() {
    return currentDateEdit->date();
}

void CalendarWidget::setupConnection() {
            foreach(auto
                            connectionManager, connectionManagers) {
            QObject::connect(connectionManager.get(), &ConnectionManager::calendarReady, this,
                             &CalendarWidget::onCalendarReady); //Connect
            QObject::connect(connectionManager.get(), &ConnectionManager::ctagChanged, this,
                             &CalendarWidget::getCalendarRequest); //Connect
            getCalendarRequest(connectionManager->getCalendarDisplayName());
        }
}

void CalendarWidget::setupTimer() {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CalendarWidget::onTimeout);
    timer->start(timerInterval);
}

void CalendarWidget::onCalendarReady(QNetworkReply *reply) {
    if (reply != nullptr) {
        QByteArray answer = reply->readAll();
        QString answerString = QString::fromUtf8(answer);
        QNetworkReply::NetworkError error = reply->error();
        if (error == QNetworkReply::NoError) {
            parseCalendar(answerString);
        } else {
            // error
            const QString &errorString = reply->errorString();
            QMessageBox::warning(this, "Error", "Could not get selected calendar");
        }
        reply->deleteLater();
    } else {
        QMessageBox::warning(this, "Error", "Something went wrong");
    }
    reply->deleteLater();
    timer->start(timerInterval);
}

void CalendarWidget::onTimeout() {
            foreach(auto
                            connectionManager, connectionManagers) {
            connectionManager->getctag();
        }
}

void CalendarWidget::onTaskDeleted(const CalendarObject &obj) {
    timer->stop();
    std::shared_ptr<ConnectionManager> connectionManager = connectionManagers[obj.getCalendarName()];
    connectionManager->getctag();
}


void CalendarWidget::shareCalendarButtonClicked() {
    shareCalendarButton->setEnabled(false);

    ShareCalendarForm *shareCalendarForm = new ShareCalendarForm(this, connectionManagers);
    shareCalendarForm->show();
    connect(shareCalendarForm, &ShareCalendarForm::shareCalendarFormClosed, this,
            &CalendarWidget::onShareCalendarFormClosed);
}

void CalendarWidget::onShareCalendarFormClosed() {
    shareCalendarButton->setEnabled(true);
}

void CalendarWidget::onTaskFormClosed() {
    addCalendarObjectButton->setEnabled(true);
}

void CalendarWidget::onTaskModified(const QString &calendarName) {
    timer->stop();
    std::shared_ptr<ConnectionManager> connectionManager = connectionManagers[calendarName];
    connectionManager->getctag();
}

std::shared_ptr<CalendarObject> parseCalendarObject_parallel(const QString &calendarObjectString) {
    QTextStream textStream(calendarObjectString.toStdString().data());
    QString line;
    QString lastKey = "";
    QString lastLine = "";
    std::shared_ptr<CalendarObject> calendarObject;
    unsigned int type;
    if (calendarObjectString.contains(QLatin1String("VEVENT"))) {
        type = 0;
    } else {
        type = 1;
    }
    if (type == 0) {
        //event
        calendarObject = std::make_shared<CalendarEvent>();
    } else {
        //to-do
        calendarObject = std::make_shared<CalendarToDo>();
    }
    while (textStream.readLineInto(&line)) {
        if (line.contains(QByteArray("END:VEVENT")) || line.contains(QByteArray("END:VTODO"))) {
            if (calendarObject->getName() != "") {
                calendarObject->setCalendarName(calendarObjectString);
                return calendarObject;
            }
            return calendarObject;
        }
        const int deliminatorPosition = line.indexOf(QLatin1Char(':'));
        const QString key = line.mid(0, deliminatorPosition);
        QString value = (line.mid(deliminatorPosition + 1, -1).replace("\\n", "\n")); //.toLatin1();
        if (key.startsWith(QLatin1String("DTSTAMP"))) {
            calendarObject->setCreationDateTime(
                    Utils::getDateTimeFromString(value).toLocalTime());
        } else if (key.startsWith(QLatin1String("DTSTART"))) {
            if (type == 0) {
                (std::dynamic_pointer_cast<CalendarEvent>(calendarObject))->setStartDateTime(
                        Utils::getDateTimeFromString(value).toLocalTime());
            } else {
                (std::dynamic_pointer_cast<CalendarToDo>(calendarObject))->setStartDateTime(
                        Utils::getDateTimeFromString(value).toLocalTime());
            }
        } else if (key.startsWith(QLatin1String("DTEND")) && type == 0) { //Only for event
            (std::dynamic_pointer_cast<CalendarEvent>(calendarObject))->setEndDateTime(
                    Utils::getDateTimeFromString(value).toLocalTime());
        } else if (key.startsWith(QLatin1String("UNTIL"))) {
            calendarObject->setUntilDateRepetition(Utils::getDateTimeFromString(value).date());
        } else if (key == QLatin1String("SUMMARY")) {
            calendarObject->setName(value);
        } else if (key == QLatin1String("LOCATION")) {
            calendarObject->setLocation(value);
        } else if (key == QLatin1String("PRIORITY")) {
            calendarObject->setPriority(value.toInt());
        } else if (key == QLatin1String("UID")) {
            calendarObject->setUID(value);
        } else if (key == QLatin1String("DESCRIPTION")) {
            lastKey = "DESCRIPTION";
            calendarObject->setDescription(value);
        } else if (key == QLatin1String("RRULE")) {
            const QString &rrule = value;

            const int deliminatorPosition2 = rrule.indexOf(QLatin1Char(';'));
            const QString typeRepString = rrule.mid(1, deliminatorPosition2 - 1);

            const QString numRepString = rrule.mid(deliminatorPosition2 + 1, -1);
            const int deliminatorPosition3 = typeRepString.indexOf(QLatin1Char('='));

            const QString typeRepetition = typeRepString.mid(deliminatorPosition3 + 1, -1);
            if (typeRepetition == QLatin1String("DAILY")) {
                calendarObject->setTypeRepetition(CalendarObject::RepetitionType::DAILY);
            } else if (typeRepetition == QLatin1String("WEEKLY")) {
                calendarObject->setTypeRepetition(CalendarObject::RepetitionType::WEEKLY);
            } else if (typeRepetition == QLatin1String("MONTHLY")) {
                calendarObject->setTypeRepetition(CalendarObject::RepetitionType::MONTHLY);
            } else if (typeRepetition == QLatin1String("YEARLY")) {
                calendarObject->setTypeRepetition(CalendarObject::RepetitionType::YEARLY);
            } else {
                calendarObject->setTypeRepetition(CalendarObject::RepetitionType::NONE);
            }
            const int deliminatorPosition4 = numRepString.indexOf(QLatin1Char('='));
            const int numRepetition = numRepString.mid(deliminatorPosition4 + 1, -1).toInt();
            calendarObject->setNumRepetition(numRepetition);
        } else if (key == QLatin1String("EXDATE")) {
            lastKey = QLatin1String("EXDATE");
            lastLine = Utils::addExDatesToCalendarObject(calendarObject.get(), value);
        } else if (key == QLatin1String("COMPLETED") && type == 1) {
            lastLine = Utils::addCompletedDatesToCalendarObject(
                    std::dynamic_pointer_cast<CalendarToDo>(calendarObject).get(),
                    value);
            lastKey = QLatin1String("COMPLETED");
        } else if (deliminatorPosition == -1) {
            if (lastKey == QLatin1String("DESCRIPTION")) {
                QString description = calendarObject->getDescription();
                description.append(value);
                calendarObject->setDescription(description);
            } else if (lastKey == QLatin1String("EXDATE")) {
                lastLine = Utils::addExDatesToCalendarObject(calendarObject.get(),
                                                             lastLine.append(value).replace(" ", ""));
            } else if (lastKey == QLatin1String("COMPLETED") && type == 1) {
                lastLine = Utils::addCompletedDatesToCalendarObject(
                        std::dynamic_pointer_cast<CalendarToDo>(calendarObject).get(),
                        lastLine.append(value).replace(" ", ""));
            }
        }
    }
    return calendarObject;
}

