#include "calendarwidget.h"

#include <memory>

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

    QDockWidget *dockWidget = new QDockWidget(tr("Dock Widget"), this);
    dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
    dockWidget->setWidget(tasksGroupBox);
    addDockWidget(Qt::RightDockWidgetArea, dockWidget);

    setMinimumHeight(480);
    setWindowTitle(tr("Calendar Application"));

}

CalendarWidget::~CalendarWidget() {
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
    //QLocale locale = QLocale(QLocale::Italian, QLocale::Italy); // set the locale you want here
    QString englishDate = date.toString("dddd, yyyy/MM/d");

    dateString->setText(englishDate);
    tasksLayout->addWidget(dateString);

    //QSize size = dateString->document()->size().toSize();
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
        showSelectedDateTasks();
    }
}




void CalendarWidget::createCalendarGroupBox() {
    calendarGroupBox = new QGroupBox(tr("CalendarWidget"));

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
    while (stream->readLineInto(&line)) {
        if (line.contains("X-WR-CALNAME")) {
            // saves calendar name
            const int deliminatorPosition = line.indexOf(QLatin1Char(':'));
            calendarName = line.mid(deliminatorPosition + 1, -1);
            int i = 0;
            while (i < calendarObjects.length()) {
                if (calendarObjects[i]->getCalendarName() == calendarName) {
                    calendarObjects.removeAt(i);
                } else {
                    i++;
                }
            }
        }
        if (line.contains("BEGIN:VEVENT")) {
            parseEvent(calendarName);
        } else if (line.contains("BEGIN:VTODO")) {
            parseToDo(calendarName);
        }
    }


    std::sort(calendarObjects.begin(), calendarObjects.end(),
              [](std::shared_ptr<CalendarObject> a, std::shared_ptr<CalendarObject> b) {
                  return a.get()->getPriority() > b.get()->getPriority();
              });

    stream->seek(0);

    this->calendar->setCalendarObjects(calendarObjects);
    showSelectedDateTasks();

}

void CalendarWidget::showSelectedDateTasks() {
    QLayoutItem *item;
    while ((item = taskViewLayout->layout()->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    for (int i = 0; i < calendarObjects.length(); i++) {
        if (calendarObjects[i]->getExDates().contains(calendar->selectedDate())) {
            continue;
        }
        std::shared_ptr<CalendarEvent> calendarEvent = std::dynamic_pointer_cast<CalendarEvent>(calendarObjects[i]);
        if (calendarEvent) {
            if (calendarEvent->getStartDateTime().date() <= calendar->selectedDate() &&
                calendarEvent->getEndDateTime().date() >= calendar->selectedDate()) {
                addCalendarObjectWidget(calendarObjects[i]);
            } else if (calendarEvent->getTypeRepetition() != CalendarObject::RepetitionType::NONE && calendarEvent->getNumRepetition() > 0) {
                QDateTime start = calendarEvent->getStartDateTime();
                QDateTime end = calendarEvent->getEndDateTime();
                switch (calendarEvent->getTypeRepetition()) {
                    case CalendarObject::RepetitionType::DAILY: {   //daily
                        while (start.date() < calendar->selectedDate() &&
                               start.date() <= calendarEvent->getUntilDateRepetition()) {
                            start = start.addDays(calendarEvent->getNumRepetition());
                            end = end.addDays(calendarEvent->getNumRepetition());
                            if (start.date() == calendar->selectedDate() &&
                                start.date() <= calendarEvent->getUntilDateRepetition()) {
                                std::shared_ptr<CalendarEvent> calendarEvent_ = std::make_shared<CalendarEvent>(
                                        calendarEvent);
                                calendarEvent_->setStartDateTime(start);
                                calendarEvent_->setEndDateTime(end);
                                addCalendarObjectWidget(calendarEvent_);
                                break;
                            }
                        }
                        break;
                    }
                    case CalendarObject::RepetitionType::WEEKLY: {      //weekly
                        while (start.date() < calendar->selectedDate() &&
                               start.date() <= calendarEvent->getUntilDateRepetition()) {
                            start = start.addDays(7 * calendarEvent->getNumRepetition());
                            end = end.addDays(7 * calendarEvent->getNumRepetition());
                            if (start.date() == calendar->selectedDate() &&
                                start.date() <= calendarEvent->getUntilDateRepetition()) {
                                std::shared_ptr<CalendarEvent> calendarEvent_ = std::make_shared<CalendarEvent>(
                                        calendarEvent);
                                calendarEvent_->setStartDateTime(start);
                                calendarEvent_->setEndDateTime(end);
                                addCalendarObjectWidget(calendarEvent_);
                                break;
                            }
                        }
                        break;
                    }
                    case CalendarObject::RepetitionType::MONTHLY: {      //monthly
                        while (start.date() < calendar->selectedDate() &&
                               start.date() <= calendarEvent->getUntilDateRepetition()) {
                            start = start.addMonths(calendarEvent->getNumRepetition());
                            end = end.addMonths(calendarEvent->getNumRepetition());
                            if (start.date() == calendar->selectedDate() &&
                                start.date() <= calendarEvent->getUntilDateRepetition()) {
                                std::shared_ptr<CalendarEvent> calendarEvent_ = std::make_shared<CalendarEvent>(
                                        calendarEvent);
                                calendarEvent_->setStartDateTime(start);
                                calendarEvent_->setEndDateTime(end);
                                addCalendarObjectWidget(calendarEvent_);
                                break;
                            }
                        }
                        break;
                    }
                    case CalendarObject::RepetitionType::YEARLY: {      //yearly
                        while (start.date() < calendar->selectedDate() &&
                               start.date() <= calendarEvent->getUntilDateRepetition()) {
                            start = start.addYears(calendarEvent->getNumRepetition());
                            end = end.addYears(calendarEvent->getNumRepetition());
                            if (start.date() == calendar->selectedDate() &&
                                start.date() <= calendarEvent->getUntilDateRepetition()) {
                                std::shared_ptr<CalendarEvent> calendarEvent_ = std::make_shared<CalendarEvent>(
                                        calendarEvent);
                                calendarEvent_->setStartDateTime(start);
                                calendarEvent_->setEndDateTime(end);
                                addCalendarObjectWidget(calendarEvent_);
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else {
            std::shared_ptr<CalendarToDo> calendarToDo = std::dynamic_pointer_cast<CalendarToDo>(calendarObjects[i]);
            QDateTime start;
            start = calendarToDo->getStartDateTime();
            if (start.date() <= calendar->selectedDate()) {
                if (start.date() == calendar->selectedDate()) {
                    addCalendarObjectWidget(calendarToDo);
                } else if (calendarToDo->getTypeRepetition() != -1 && calendarToDo->getNumRepetition() > 0) {
                    if (calendarToDo->getUntilDateRepetition() >= calendar->selectedDate()) {
                        //std::cout << "Checking recurrences for " << calendarToDo->getName().toStdString() << '\n';
                        switch (calendarToDo->getTypeRepetition()) {
                            case CalendarObject::RepetitionType::DAILY: {   // daily
                                while (start.date() < calendar->selectedDate() &&
                                       start.date() <= calendarToDo->getUntilDateRepetition()) {
                                    start = start.addDays(calendarToDo->getNumRepetition());
                                    if (start.date() == calendar->selectedDate() &&
                                        start.date() <= calendarToDo->getUntilDateRepetition()) {
                                        std::shared_ptr<CalendarToDo> calendarToDo_ = std::make_shared<CalendarToDo>(
                                                calendarToDo);
                                        calendarToDo_->setStartDateTime(start);
                                        addCalendarObjectWidget(calendarToDo_);
                                        break;
                                    }
                                }
                                break;
                            }
                            case CalendarObject::RepetitionType::WEEKLY: {      //weekly
                                while (start.date() < calendar->selectedDate() &&
                                       start.date() <= calendarToDo->getUntilDateRepetition()) {
                                    start = start.addDays(7 * calendarToDo->getNumRepetition());
                                    if (start.date() == calendar->selectedDate() &&
                                        start.date() <= calendarToDo->getUntilDateRepetition()) {
                                        std::shared_ptr<CalendarObject> calendarToDo_ = std::make_shared<CalendarToDo>(
                                                calendarToDo);
                                        calendarToDo_->setStartDateTime(start);
                                        addCalendarObjectWidget(calendarToDo_);
                                        break;
                                    }
                                }
                                break;
                            }
                            case CalendarObject::RepetitionType::MONTHLY: {      //monthly
                                while (start.date() < calendar->selectedDate() &&
                                       start.date() <= calendarToDo->getUntilDateRepetition()) {
                                    start = start.addMonths(calendarToDo->getNumRepetition());
                                    if (start.date() == calendar->selectedDate() &&
                                        start.date() <= calendarToDo->getUntilDateRepetition()) {
                                        std::shared_ptr<CalendarToDo> calendarToDo_ = std::make_shared<CalendarToDo>(
                                                calendarToDo);
                                        calendarToDo_->setStartDateTime(start);
                                        addCalendarObjectWidget(calendarToDo_);
                                        break;
                                    }
                                }
                                break;
                            }
                            case CalendarObject::RepetitionType::YEARLY: {      //yearly
                                while (start.date() < calendar->selectedDate() &&
                                       start.date() <= calendarToDo->getUntilDateRepetition()) {
                                    start = start.addYears(calendarToDo->getNumRepetition());
                                    if (start.date() == calendar->selectedDate() &&
                                        start.date() <= calendarToDo->getUntilDateRepetition()) {
                                        std::shared_ptr<CalendarToDo> calendarToDo_ = std::make_shared<CalendarToDo>(
                                                calendarToDo);
                                        calendarToDo_->setStartDateTime(start);
                                        addCalendarObjectWidget(calendarToDo_);
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                    }

                }
            }
        }
    }
}

void CalendarWidget::addCalendarObjectWidget(std::shared_ptr<CalendarObject> calendarObject) {
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

void CalendarWidget::parseEvent(const QString &calendarName) {
    QString line;
    QString lastKey = "";
    QString lastLine = "";
    std::shared_ptr<CalendarObject> calendarObject = std::make_shared<CalendarEvent>();
    while (stream->readLineInto(&line)) {
        if (line.contains(QByteArray("END:VEVENT"))) {
            if (calendarObject->getName() != "") {
                calendarObject->setCalendarName(calendarName);
                calendarObjects.append(calendarObject);
            }
            return;
        }
        const int deliminatorPosition = line.indexOf(QLatin1Char(':'));
        const QString key = line.mid(0, deliminatorPosition);
        QString value = (line.mid(deliminatorPosition + 1, -1).replace("\\n", "\n")); //.toLatin1();
        if (key.startsWith(QLatin1String("DTSTAMP"))) {
            calendarObject->setCreationDateTime(
                    getDateTimeFromString(value).toLocalTime());
        } else if (key.startsWith(QLatin1String("DTSTART"))) {
            (std::dynamic_pointer_cast<CalendarEvent>(calendarObject))->setStartDateTime(
                    getDateTimeFromString(value).toLocalTime());
        } else if (key.startsWith(QLatin1String("DTEND"))) {
            (std::dynamic_pointer_cast<CalendarEvent>(calendarObject))->setEndDateTime(
                    getDateTimeFromString(value).toLocalTime());
        } else if (key.startsWith(QLatin1String("UNTIL"))) {
            calendarObject->setUntilDateRepetition(getDateTimeFromString(value).date());
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
            const QString rrule = value;

            const int deliminatorPosition2 = rrule.indexOf(QLatin1Char(';'));
            const QString typeRepString = rrule.mid(1, deliminatorPosition2 - 1);

            const QString numRepString = rrule.mid(deliminatorPosition2 + 1, -1);
            const int deliminatorPosition3 = typeRepString.indexOf(QLatin1Char('='));

            const QString typeRepetition = typeRepString.mid(deliminatorPosition3 + 1, -1);
            if (typeRepetition == "DAILY") {
                calendarObject->setTypeRepetition(CalendarObject::RepetitionType::DAILY);
            } else if (typeRepetition == "WEEKLY") {
                calendarObject->setTypeRepetition(CalendarObject::RepetitionType::WEEKLY);
            } else if (typeRepetition == "MONTHLY") {
                calendarObject->setTypeRepetition(CalendarObject::RepetitionType::MONTHLY);
            } else if (typeRepetition == "YEARLY") {
                calendarObject->setTypeRepetition(CalendarObject::RepetitionType::YEARLY);
            } else {
                calendarObject->setTypeRepetition(CalendarObject::RepetitionType::NONE);
            }
            const int deliminatorPosition4 = numRepString.indexOf(QLatin1Char('='));
            const int numRepetition = numRepString.mid(deliminatorPosition4 + 1, -1).toInt();
            calendarObject->setNumRepetition(numRepetition);
        } else if (key == "EXDATE") {
            lastKey = "EXDATE";
            lastLine = addExDatesToCalendarObject(calendarObject.get(), value);
        } else if (deliminatorPosition == -1) {
            if (lastKey == "DESCRIPTION") {
                QString description = calendarObject->getDescription();
                description.append(value);
                calendarObject->setDescription(description);
            } else if (lastKey == "EXDATE") {
                lastLine = addExDatesToCalendarObject(calendarObject.get(), lastLine.append(value).replace(" ", ""));
            }
        }
    }
}

void CalendarWidget::parseToDo(const QString &calendarName) {
    QString line;
    std::shared_ptr<CalendarObject> calendarObject(new CalendarToDo());
    QString lastKey = "";
    QString lastLine = "";
    while (stream->readLineInto(&line)) {
        //std::cout << line.toStdString()<<'\n';
        if (line.contains(QByteArray("END:VTODO"))) {
            if (calendarObject->getName() != "") {
                calendarObject->setCalendarName(calendarName);
                calendarObjects.append(std::shared_ptr<CalendarObject>(calendarObject));
            }
            return;
        }
        const int deliminatorPosition = line.indexOf(QLatin1Char(':'));
        const QString key = line.mid(0, deliminatorPosition);
        QString value = (line.mid(deliminatorPosition + 1, -1).replace("\\n", "\n")); //.toLatin1();
        if (key.startsWith(QLatin1String("DTSTAMP"))) {
            calendarObject->setCreationDateTime(
                    getDateTimeFromString(value).toLocalTime());
        } else if (key.startsWith(QLatin1String("DTSTART"))) {
            (std::dynamic_pointer_cast<CalendarToDo>(calendarObject))->setStartDateTime(
                    getDateTimeFromString(value).toLocalTime());
        } else if (key.startsWith(QLatin1String("UNTIL"))) {
            calendarObject->setUntilDateRepetition(getDateTimeFromString(value).date());
        } else if (key == QLatin1String("SUMMARY")) {
            calendarObject->setName(value);
        } else if (key == QLatin1String("LOCATION")) {
            calendarObject->setLocation(value);
        } else if (key == QLatin1String("UID")) {
            calendarObject->setUID(value);
        } else if (key == QLatin1String("DESCRIPTION")) {
            calendarObject->setDescription(value);
            lastKey = "DESCRIPTION";
        } else if (deliminatorPosition == -1) {
            if (lastKey == "DESCRIPTION") {
                QString description = calendarObject->getDescription();
                description.append(value);
                calendarObject->setDescription(description);
            } else if (lastKey == "EXDATE") {
                lastLine = addExDatesToCalendarObject(calendarObject.get(),
                                                      lastLine.append(value).replace(" ", ""));
            } else if (lastKey == "COMPLETED") {
                lastLine = addCompletedDatesToCalendarObject(
                        std::dynamic_pointer_cast<CalendarToDo>(calendarObject).get(),
                        lastLine.append(value).replace(" ", ""));
            }

        } else if (key == QLatin1String("PRIORITY")) {
            calendarObject->setPriority(value.toInt());
        } else if (key == QLatin1String("COMPLETED")) {
            lastLine = addCompletedDatesToCalendarObject(
                    std::dynamic_pointer_cast<CalendarToDo>(calendarObject).get(),
                    value);
            lastKey = "COMPLETED";
        } else if (key == QLatin1String("RRULE")) {
            QString rrule = value;

            const int deliminatorPosition2 = rrule.indexOf(QLatin1Char(';'));
            const QString typeRepString = rrule.mid(1, deliminatorPosition2 - 1);

            const QString numRepString = rrule.mid(deliminatorPosition2 + 1, -1);
            const int deliminatorPosition3 = typeRepString.indexOf(QLatin1Char('='));

            const QString typeRepetition = typeRepString.mid(deliminatorPosition3 + 1, -1);
            if (typeRepetition == "DAILY") {
                calendarObject->setTypeRepetition(CalendarObject::RepetitionType::DAILY);
            } else if (typeRepetition == "WEEKLY") {
                calendarObject->setTypeRepetition(CalendarObject::RepetitionType::WEEKLY);
            } else if (typeRepetition == "MONTHLY") {
                calendarObject->setTypeRepetition(CalendarObject::RepetitionType::MONTHLY);
            } else if (typeRepetition == "YEARLY") {
                calendarObject->setTypeRepetition(CalendarObject::RepetitionType::YEARLY);
            } else {
                calendarObject->setTypeRepetition(CalendarObject::RepetitionType::NONE);
            }
            const int deliminatorPosition4 = numRepString.indexOf(QLatin1Char('='));
            const int numRepetition = numRepString.mid(deliminatorPosition4 + 1, -1).toInt();
            calendarObject->setNumRepetition(numRepetition);
        } else if (key == QLatin1String("EXDATE")) {
            addExDatesToCalendarObject(calendarObject.get(), value);
            lastKey = "EXDATE";
        }
    }
}

QString CalendarWidget::addExDatesToCalendarObject(CalendarObject *calendarObject, QString &value) {
    int endDeliminatorPosition = value.indexOf(QLatin1Char('Z'));
    QList<QDate> exDates;
    while (endDeliminatorPosition > 0 && !value.isEmpty()) {
        const QString exDateString = value.mid(0, endDeliminatorPosition + 1);
        const QDate exDate = getDateTimeFromString(exDateString).toLocalTime().date();
        calendarObject->addExDate(exDate);
        value = value.mid(endDeliminatorPosition + 2, -1);
        endDeliminatorPosition = value.indexOf(QLatin1Char('Z'));
    }
    return value;
}

QString CalendarWidget::addCompletedDatesToCalendarObject(CalendarToDo *calendarTodo, QString &value) {
    int endDeliminatorPosition = value.indexOf(QLatin1Char('Z'));
    while (endDeliminatorPosition > 0 && !value.isEmpty()) {
        const QString completedDateString = value.mid(0, endDeliminatorPosition + 1);
        const QDate completedDate = getDateTimeFromString(completedDateString).toLocalTime().date();
        calendarTodo->addCompletedDate(completedDate);
        value = value.mid(endDeliminatorPosition + 2, -1);
        endDeliminatorPosition = value.indexOf(QLatin1Char('Z'));
    }
    return value;
}

void CalendarWidget::addCalendarObjectButtonClicked() {
    addCalendarObjectButton->setEnabled(false);
    TaskForm *taskForm = new TaskForm(connectionManagers);
    taskForm->setDate(currentDateEdit->date());
    taskForm->show();
    connect(taskForm, &TaskForm::taskFormClosed, this, &CalendarWidget::onTaskFormClosed);
    connect(taskForm, &TaskForm::taskUploaded, this, &CalendarWidget::onTaskModified);
}

QDateTime CalendarWidget::getDateTimeFromString(const QString &string) {
    QDateTime dateTime = QDateTime::fromString(string, "yyyyMMdd'T'hhmmss'Z'");
    if (!dateTime.isValid())
        dateTime = QDateTime::fromString(string, "yyyyMMdd'T'hhmmss");
    if (!dateTime.isValid())
        dateTime = QDateTime::fromString(string, "yyyyMMddhhmmss");
    if (!dateTime.isValid())
        dateTime = QDateTime::fromString(string, "yyyyMMdd");
    if (!dateTime.isValid())
        std::cerr << "CalendarWidget: could not parse " << string.toStdString() << '\n';
    return dateTime;
}


void CalendarWidget::addConnectionManager(ConnectionManager *connectionManager) {
    QString calendarName = connectionManager->getCalendarName();
    if (!connectionManagers.contains(calendarName)) {
        connectionManagers.insert(calendarName, std::shared_ptr<ConnectionManager>(connectionManager));
    }
}


void CalendarWidget::getCalendarRequest(const QString calendarName) {
    std::shared_ptr<ConnectionManager> connectionManager = connectionManagers[calendarName];
    connectionManager->getCalendarRequest();
}

QDate CalendarWidget::getCurrentDateSelected() {
    return currentDateEdit->date();
}

void CalendarWidget::setupConnection() {
            foreach(auto connectionManager, connectionManagers) {
            QObject::connect(connectionManager.get(), &ConnectionManager::calendarReady, this,
                             &CalendarWidget::onCalendarReady); //Connect
            getCalendarRequest(connectionManager->getCalendarName());
        };
}

void CalendarWidget::setupTimer() {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CalendarWidget::onTimeout);
            foreach(auto connectionManager, connectionManagers) {
            QObject::connect(connectionManager.get(), &ConnectionManager::ctagChanged, this,
                             &CalendarWidget::getCalendarRequest); //Connect
        };
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
            std::cerr << errorString.toStdString() << '\n';
            QMessageBox::warning(this, "Error", "Could not get selected calendar");
        }
        reply->deleteLater();
    } else {
        QMessageBox::warning(this, "Error", "Something went wrong");
    }
    timer->start(timerInterval);
}

void CalendarWidget::onTimeout() {
            foreach(auto connectionManager, connectionManagers) {
            connectionManager->getctag();
        };
}
void CalendarWidget::onTaskDeleted(CalendarObject &obj) {
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

void CalendarWidget::onTaskModified(const QString calendarName) {
    timer->stop();
    std::shared_ptr<ConnectionManager> connectionManager = connectionManagers[calendarName];
    connectionManager->getctag();
}

