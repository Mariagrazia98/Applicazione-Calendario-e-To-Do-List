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

    QDockWidget * dockWidget = new QDockWidget(tr("Dock Widget"), this);
    dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
    dockWidget->setWidget(tasksGroupBox);
    dockWidget->setFeatures(dockWidget->features() & ~QDockWidget::DockWidgetClosable);
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
            /* TODO che fa sto while? */
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


void CalendarWidget::showSelectedDateCalendarObjects() {
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
        /* this is a Calendar Event */
        if (calendarEvent) {
            if (calendarEvent->getStartDateTime().date() <= calendar->selectedDate() &&
                calendarEvent->getEndDateTime().date() >= calendar->selectedDate()) {
                addCalendarObjectWidget(calendarObjects[i]);
            } else if (calendarEvent->getTypeRepetition() != CalendarObject::RepetitionType::NONE &&
                       calendarEvent->getNumRepetition() > 0) {
                QDateTime start = calendarEvent->getStartDateTime();
                QDateTime end = calendarEvent->getEndDateTime();

                while (start.date() < calendar->selectedDate() &&
                       start.date() <= calendarEvent->getUntilDateRepetition()) {

                    /*
                     * if the event takes more than one day i have to readjust the dates
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
                    if (start.date() <= calendar->selectedDate() &&
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
            std::shared_ptr<CalendarToDo> calendarToDo = std::dynamic_pointer_cast<CalendarToDo>(calendarObjects[i]);
            QDateTime start;
            start = calendarToDo->getStartDateTime();
            if (start.date() <= calendar->selectedDate()) {
                if (start.date() == calendar->selectedDate()) {
                    addCalendarObjectWidget(calendarToDo);
                } else if (calendarToDo->getTypeRepetition() != -1 && calendarToDo->getNumRepetition() > 0) {
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
    CalendarObjectForm *taskForm = new CalendarObjectForm(connectionManagers);
    taskForm->setDate(currentDateEdit->date());
    taskForm->show();
    connect(taskForm, &CalendarObjectForm::taskFormClosed, this, &CalendarWidget::onTaskFormClosed);
    connect(taskForm, &CalendarObjectForm::taskUploaded, this, &CalendarWidget::onTaskModified);
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
            foreach(auto
                            connectionManager, connectionManagers) {
            QObject::connect(connectionManager.get(), &ConnectionManager::calendarReady, this,
                             &CalendarWidget::onCalendarReady); //Connect
            QObject::connect(connectionManager.get(), &ConnectionManager::ctagChanged, this,
                             &CalendarWidget::getCalendarRequest); //Connect
            getCalendarRequest(connectionManager->getCalendarName());
        };
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
            foreach(auto
                            connectionManager, connectionManagers) {
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


void CalendarWidget::parseCalendarObject(const QString &calendarName, unsigned int type) {
    QString line;
    QString lastKey = "";
    QString lastLine = "";
    std::shared_ptr<CalendarObject> calendarObject;
    if (type == 0) {
        //event
        calendarObject = std::make_shared<CalendarEvent>();
    } else {
        //to-do
        calendarObject = std::make_shared<CalendarToDo>();
    }
    while (stream->readLineInto(&line)) {
        if (line.contains(QByteArray("END:VEVENT")) || line.contains(QByteArray("END:VTODO"))) {
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
            if (type == 0) {
                (std::dynamic_pointer_cast<CalendarEvent>(calendarObject))->setStartDateTime(
                        getDateTimeFromString(value).toLocalTime());
            } else {
                (std::dynamic_pointer_cast<CalendarToDo>(calendarObject))->setStartDateTime(
                        getDateTimeFromString(value).toLocalTime());
            }
        } else if (key.startsWith(QLatin1String("DTEND")) && type == 0) { //Only for event
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
        } else if (key == QLatin1String("COMPLETED") && type == 1) {
            lastLine = addCompletedDatesToCalendarObject(
                    std::dynamic_pointer_cast<CalendarToDo>(calendarObject).get(),
                    value);
            lastKey = "COMPLETED";
        } else if (deliminatorPosition == -1) {
            if (lastKey == "DESCRIPTION") {
                QString description = calendarObject->getDescription();
                description.append(value);
                calendarObject->setDescription(description);
            } else if (lastKey == "EXDATE") {
                lastLine = addExDatesToCalendarObject(calendarObject.get(),
                                                      lastLine.append(value).replace(" ", ""));
            } else if (lastKey == "COMPLETED" && type == 1) {
                lastLine = addCompletedDatesToCalendarObject(
                        std::dynamic_pointer_cast<CalendarToDo>(calendarObject).get(),
                        lastLine.append(value).replace(" ", ""));
            }
        }
    }
}

