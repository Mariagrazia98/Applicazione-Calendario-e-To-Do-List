#include "calendarwidget.h"
#include "ui_calendarwidget.h"

#define DAILY 1
#define WEEKLY 2
#define MONTHLY 3
#define YEARLY 4

CalendarWidget::CalendarWidget(QWidget *parent, ConnectionManager *connectionManager) :
        QWidget(parent),
        ui(new Ui::CalendarWidget),
        dateString(new QTextBrowser),
        connectionManager(connectionManager),
        stream(new QTextStream()),
        timerInterval(10000) {
    ui->setupUi(this);
    createCalendarGroupBox();
    setupCalendar();

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(calendarGroupBox, 0, 0);
    layout->addWidget(tasksGroupBox, 0, 1);

    //layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);

    layout->setRowMinimumHeight(0, calendar->sizeHint().height());
    layout->setColumnMinimumWidth(0, calendar->sizeHint().width());
    layout->setColumnMinimumWidth(1, calendar->sizeHint().width() * 1.5);

    setMinimumHeight(480);

    setWindowTitle(tr("Calendar Application"));

}

CalendarWidget::~CalendarWidget() {
    delete ui;
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

    setupWeek();
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

    addTaskButton = new QPushButton(tr("&Add"));
    addTaskButton->setEnabled(true);
    addTaskButton->setToolTip(tr("Add new task"));
    connect(addTaskButton, &QPushButton::clicked, this, &CalendarWidget::addTaskButtonClicked);
    tasksLayout->addWidget(addTaskButton);
}

void CalendarWidget::selectedDateChanged() {
    if (currentDateEdit->date() != calendar->selectedDate()) {
        currentDateEdit->setDate(calendar->selectedDate());
        QDate date = currentDateEdit->date();
        //QLocale locale = QLocale(QLocale::Italian, QLocale::Italy); // set the locale you want here
        QString englishDate = date.toString("dddd, yyyy/MM/d");
        dateString->setText(englishDate);
        dateString->setAlignment(Qt::AlignCenter);
        showSelectedDateTasks();
    }
}

void CalendarWidget::setupWeek() {
    QTextCharFormat format;

    format.setForeground(Qt::black);
    calendar->setWeekdayTextFormat(Qt::Monday, format);
    calendar->setWeekdayTextFormat(Qt::Tuesday, format);
    calendar->setWeekdayTextFormat(Qt::Wednesday, format);
    calendar->setWeekdayTextFormat(Qt::Thursday, format);
    calendar->setWeekdayTextFormat(Qt::Friday, format);
    calendar->setWeekdayTextFormat(Qt::Saturday, format);
    format.setForeground(Qt::red);
    calendar->setWeekdayTextFormat(Qt::Sunday, format);
}

void CalendarWidget::reformatCalendarPage() {
    QTextCharFormat mayFirstFormat;
    const QDate mayFirst(calendar->yearShown(), 5, 1);

    QTextCharFormat firstFridayFormat;
    QDate firstFriday(calendar->yearShown(), calendar->monthShown(), 1);
    while (firstFriday.dayOfWeek() != Qt::Friday)
        firstFriday = firstFriday.addDays(1);

    calendar->setDateTextFormat(firstFriday, firstFridayFormat);
    calendar->setDateTextFormat(mayFirst, mayFirstFormat);
}

void CalendarWidget::createCalendarGroupBox() {
    calendarGroupBox = new QGroupBox(tr("CalendarWidget"));

    calendar = new CustomCalendarWidget();


    connect(calendar, &QCalendarWidget::currentPageChanged,
            this, &CalendarWidget::reformatCalendarPage);

    calendarLayout = new QGridLayout;
    calendarLayout->addWidget(calendar);
    calendarGroupBox->setLayout(calendarLayout);
}


void CalendarWidget::parseCalendar(QString calendar) {
    stream = new QTextStream(&calendar, QIODevice::ReadOnly);
    QString line;

    while (stream->readLineInto(&line)) {

        if (line.contains("BEGIN:VEVENT")) {
            parseEvent();
        } else if (line.contains("BEGIN:VTODO")) {
            parseToDo();
        }
    }

    std::sort(calendarObjects.begin(), calendarObjects.end(), [](CalendarObject *a, CalendarObject *b) {
        return a->getPriority() > b->getPriority();
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
        CalendarEvent *calendarEvent = dynamic_cast<CalendarEvent *>(calendarObjects[i]);
        if (calendarEvent) {
            if (calendarEvent->getStartDateTime().date() <= calendar->selectedDate() &&
                calendarEvent->getEndDateTime().date() >= calendar->selectedDate()) {
                addCalendarObjectWidget(calendarEvent);
            } else if (calendarEvent->getTypeRepetition() != -1 && calendarEvent->getNumRepetition() > 0) {
                QDateTime start = calendarEvent->getStartDateTime();
                QDateTime end = calendarEvent->getEndDateTime();
                switch (calendarEvent->getTypeRepetition()) {
                    case DAILY: {   //daily
                        while (start.date() < calendar->selectedDate()) {
                            start = start.addDays(calendarEvent->getNumRepetition());
                            end = end.addDays(calendarEvent->getNumRepetition());
                            if (start.date() == calendar->selectedDate()) {
                                CalendarEvent *calendarEvent_ = new CalendarEvent(*calendarEvent);
                                calendarEvent_->setStartDateTime(start);
                                calendarEvent_->setEndDateTime(end);
                                addCalendarObjectWidget(calendarEvent_);
                                break;
                            }
                        }
                        break;
                    }
                    case WEEKLY: {      //weekly
                        while (start.date() < calendar->selectedDate()) {
                            start = start.addDays(7 * calendarEvent->getNumRepetition());
                            end = end.addDays(7 * calendarEvent->getNumRepetition());
                            if (start.date() == calendar->selectedDate()) {
                                CalendarEvent *calendarEvent_ = new CalendarEvent(*calendarEvent);
                                calendarEvent_->setStartDateTime(start);
                                calendarEvent_->setEndDateTime(end);
                                addCalendarObjectWidget(calendarEvent_);
                                break;
                            }
                        }
                        break;
                    }
                    case MONTHLY: {      //monthly
                        while (start.date() < calendar->selectedDate()) {
                            start = start.addMonths(calendarEvent->getNumRepetition());
                            end = end.addMonths(calendarEvent->getNumRepetition());
                            if (start.date() == calendar->selectedDate()) {
                                CalendarEvent *calendarEvent_ = new CalendarEvent(*calendarEvent);
                                calendarEvent_->setStartDateTime(start);
                                calendarEvent_->setEndDateTime(end);
                                addCalendarObjectWidget(calendarEvent_);
                                break;
                            }
                        }
                        break;
                    }
                    case YEARLY: {      //yearly
                        while (start.date() < calendar->selectedDate()) {
                            start = start.addYears(calendarEvent->getNumRepetition());
                            end = end.addYears(calendarEvent->getNumRepetition());
                            if (start.date() == calendar->selectedDate()) {
                                CalendarEvent *calendarEvent_ = new CalendarEvent(*calendarEvent);
                                calendarEvent_->setStartDateTime(start);
                                calendarEvent_->setEndDateTime(end);
                                addCalendarObjectWidget(calendarEvent);
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else {
            CalendarToDo *calendarToDo = dynamic_cast<CalendarToDo *>(calendarObjects[i]);
            QDateTime start;
            start = calendarToDo->getStartDateTime();
            if (start.date() <= calendar->selectedDate()) {
                if (!(calendarToDo->getDueDateTime() &&
                      calendarToDo->getDueDateTime()->date() < calendar->selectedDate())) {
                    addCalendarObjectWidget(calendarToDo);
                } else if (calendarToDo->getTypeRepetition() != -1 && calendarToDo->getNumRepetition() > 0) {
                    switch (calendarToDo->getTypeRepetition()) {
                        case DAILY: {   //daily
                            while (start.date() < calendar->selectedDate()) {
                                start = start.addDays(calendarToDo->getNumRepetition());
                                if (start.date() == calendar->selectedDate()) {

                                    CalendarToDo *calendarToDo_ = new CalendarToDo(
                                            *calendarToDo); // TODO: usare smart ptrs?
                                    calendarToDo_->setStartDateTime(start);
                                    /* TODO calendarToDo_->setCompletedDateTime(calendarToDo->getCompletedDateTime()); */
                                    addCalendarObjectWidget(calendarToDo_);

                                    break;
                                }
                            }
                            break;
                        }
                        case WEEKLY: {      //weekly
                            while (start.date() < calendar->selectedDate()) {
                                start = start.addDays(7 * calendarToDo->getNumRepetition());
                                if (start.date() == calendar->selectedDate()) {
                                    CalendarToDo *calendarToDo_ = new CalendarToDo(*calendarToDo);
                                    calendarToDo_->setStartDateTime(start);
                                    /*TODO calendarToDo_->setCompletedDateTime(calendarToDo->getCompletedDateTime()); */
                                    addCalendarObjectWidget(calendarToDo_);
                                    break;
                                }
                            }
                            break;
                        }
                        case MONTHLY: {      //monthly
                            while (start.date() < calendar->selectedDate()) {
                                start = start.addMonths(calendarToDo->getNumRepetition());
                                if (start.date() == calendar->selectedDate()) {
                                    CalendarToDo *calendarToDo_ = new CalendarToDo(*calendarToDo);
                                    calendarToDo_->setStartDateTime(start);
                                    /* TODO calendarToDo_->setCompletedDateTime(calendarToDo->getCompletedDateTime()); */
                                    addCalendarObjectWidget(calendarToDo_);
                                    break;
                                }
                            }
                            break;
                        }
                        case YEARLY: {      //yearly
                            while (start.date() < calendar->selectedDate()) {
                                start = start.addYears(calendarToDo->getNumRepetition());
                                if (start.date() == calendar->selectedDate()) {
                                    CalendarToDo *calendarToDo_ = new CalendarToDo(*calendarToDo);
                                    calendarToDo_->setStartDateTime(start);
                                    /* TODO  calendarToDo_->setCompletedDateTime(calendarToDo->getCompletedDateTime()); */
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

void CalendarWidget::addCalendarObjectWidget(CalendarObject *calendarObject) {
    CalendarObjectWidget *calendarObjectWidget = new CalendarObjectWidget(this, *calendarObject, connectionManager);
    calendarObjectWidget->setupUI();
    calendarObjectWidget->setVisible(true);
    calendarObjectWidget->setEnabled(true);
    taskViewLayout->addWidget(calendarObjectWidget);
    connect(calendarObjectWidget, &CalendarObjectWidget::taskModified, this,
            &CalendarWidget::onTaskModified);
    connect(calendarObjectWidget, &CalendarObjectWidget::taskDeleted, this,
            &CalendarWidget::onTaskDeleted);
}

void CalendarWidget::parseEvent() {
    QString line;

    CalendarObject *calendarObject = new CalendarEvent();
    while (stream->readLineInto(&line)) {
        if (line.contains(QByteArray("END:VEVENT"))) {
            if (calendarObject->getName() != "") {
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
            dynamic_cast<CalendarEvent *>(calendarObject)->setStartDateTime(getDateTimeFromString(value).toLocalTime());
        } else if (key.startsWith(QLatin1String("DTEND"))) {
            dynamic_cast<CalendarEvent *>(calendarObject)->setEndDateTime(getDateTimeFromString(value).toLocalTime());
        } else if (key == QLatin1String("SUMMARY")) {
            calendarObject->setName(value);
        } else if (key == QLatin1String("LOCATION")) {
            calendarObject->setLocation(value);
        } else if (key == QLatin1String("PRIORITY")) {
            calendarObject->setPriority(value.toInt());
        } else if (key == QLatin1String("UID")) {
            calendarObject->setUID(value);
        } else if (key == QLatin1String("DESCRIPTION")) {
            calendarObject->setDescription(value);
        } else if (deliminatorPosition == -1) {
            QString description = calendarObject->getDescription();
            description.append(value);
            calendarObject->setDescription(description);
        } else if (key == QLatin1String("RRULE")) {
            const QString rrule = value;

            const int deliminatorPosition2 = rrule.indexOf(QLatin1Char(';'));
            const QString typeRepString = rrule.mid(1, deliminatorPosition2 - 1);

            const QString numRepString = rrule.mid(deliminatorPosition2 + 1, -1);
            const int deliminatorPosition3 = typeRepString.indexOf(QLatin1Char('='));

            const QString typeRepetition = typeRepString.mid(deliminatorPosition3 + 1, -1);

            if (typeRepetition == "DAILY") {
                calendarObject->setTypeRepetition(DAILY);
            } else if (typeRepetition == "WEEKLY") {
                calendarObject->setTypeRepetition(WEEKLY);
            } else if (typeRepetition == "MONTHLY") {
                calendarObject->setTypeRepetition(MONTHLY);
            } else if (typeRepetition == "YEARLY") {
                calendarObject->setTypeRepetition(YEARLY);
            } else {
                calendarObject->setTypeRepetition(-1);
            }

            const int deliminatorPosition4 = numRepString.indexOf(QLatin1Char('='));
            const int numRepetition = numRepString.mid(deliminatorPosition4 + 1, -1).toInt();
            calendarObject->setNumRepetition(numRepetition);
        } else if (key == QLatin1String("EXDATE")) {
            int startDelimitatorPosition = 0;
            int endDelimitatorPosition = value.indexOf(QLatin1Char('Z'));
            QList<QDate> exDates;
            while (endDelimitatorPosition != -1 || value.isEmpty()) {
                std::cout << "parsing value: " << value.toStdString() << '\n';
                const QString exDateString = value.mid(startDelimitatorPosition,
                                                       endDelimitatorPosition);
                std::cout << "exDateString: " << exDateString.toStdString() << '\n';
                const QDate exDate = getDateTimeFromString(exDateString).toLocalTime().date();
                exDates.append(exDate);
                std::cout << "ex date " + exDate.toString().toStdString() << "\n";
                value = value.mid(endDelimitatorPosition, -1);
                startDelimitatorPosition = 2;
                endDelimitatorPosition = value.indexOf(QLatin1Char('Z'));
            }
            calendarObject->setExDates(exDates);
        }
    }
}

void CalendarWidget::parseToDo() {
    QString line;
    CalendarObject *calendarObject = new CalendarToDo();
    while (stream->readLineInto(&line)) {
        if (line.contains(QByteArray("END:VTODO"))) {
            if (calendarObject->getName() != "") {
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
            dynamic_cast<CalendarToDo *>(calendarObject)->setStartDateTime(getDateTimeFromString(value).toLocalTime());
        } else if (key.startsWith(QLatin1String("DUE"))) {
            dynamic_cast<CalendarToDo *>(calendarObject)->setDueDateTime(getDateTimeFromString(value).toLocalTime());
        } else if (key == QLatin1String("SUMMARY")) {
            calendarObject->setName(value);
        } else if (key == QLatin1String("LOCATION")) {
            calendarObject->setLocation(value);
        } else if (key == QLatin1String("UID")) {
            calendarObject->setUID(value);
        } else if (key == QLatin1String("DESCRIPTION")) {
            calendarObject->setDescription(value);
        } else if (key == QLatin1String("PRIORITY")) {
            calendarObject->setPriority(value.toInt());
        } else if (key == QLatin1String("COMPLETED")) {
            dynamic_cast<CalendarToDo *>(calendarObject)->setCompletedDateTime(
                    getDateTimeFromString(value).toLocalTime());
        } else if (key == QLatin1String("RRULE")) {
            QString rrule = value;

            const int deliminatorPosition2 = rrule.indexOf(QLatin1Char(';'));
            const QString typeRepString = rrule.mid(1, deliminatorPosition2 - 1);

            const QString numRepString = rrule.mid(deliminatorPosition2 + 1, -1);
            const int deliminatorPosition3 = typeRepString.indexOf(QLatin1Char('='));

            const QString typeRepetition = typeRepString.mid(deliminatorPosition3 + 1, -1);

            if (typeRepetition == "DAILY") {
                calendarObject->setTypeRepetition(DAILY);
            } else if (typeRepetition == "WEEKLY") {
                calendarObject->setTypeRepetition(WEEKLY);
            } else if (typeRepetition == "MONTHLY") {
                calendarObject->setTypeRepetition(MONTHLY);
            } else if (typeRepetition == "YEARLY") {
                calendarObject->setTypeRepetition(YEARLY);
            } else {
                calendarObject->setTypeRepetition(-1);
            }

            const int deliminatorPosition4 = numRepString.indexOf(QLatin1Char('='));
            const int numRepetition = numRepString.mid(deliminatorPosition4 + 1, -1).toInt();
            calendarObject->setNumRepetition(numRepetition);
        }
    }
}

void CalendarWidget::addTaskButtonClicked() {
    addTaskButton->setEnabled(false);
    TaskForm *taskForm = new TaskForm(connectionManager);
    taskForm->setDate(currentDateEdit->date());
    taskForm->show();
    connect(taskForm, &TaskForm::closing, this, &CalendarWidget::onTaskFormClosed);
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
        std::cerr << "CalendarWidget" << ": " << "could not parse" << string.toStdString() << '\n';
    return dateTime;
}

void CalendarWidget::onTaskFormClosed() {
    addTaskButton->setEnabled(true);
    // TODO: aggiornare widgets se ce n'è bisogno
}

void CalendarWidget::onTaskModified() {
    timer->stop();
    connectionManager->getctag();
}

void CalendarWidget::setConnectionManager(ConnectionManager *connectionManager) {
    CalendarWidget::connectionManager = connectionManager;
}

void CalendarWidget::onCalendarReady(QNetworkReply *reply) {
    if (reply != nullptr) {
        QByteArray answer = reply->readAll();
        QString answerString = QString::fromUtf8(answer);
        QNetworkReply::NetworkError error = reply->error();
        if (error == QNetworkReply::NoError) {
            calendarObjects.clear();
            parseCalendar(answerString);
        } else {
            // error
            const QString &errorString = reply->errorString();
            std::cerr << errorString.toStdString() << '\n';
            QMessageBox::warning(this, "Error", "Could not get selected calendar");
        }
        reply->deleteLater();
    } else {
        //std::cerr << "null reply\n";
        QMessageBox::warning(this, "Error", "Something went wrong");
    }
    timer->start(timerInterval);
}

void CalendarWidget::getCalendarRequest() {
    connectionManager->getCalendarRequest();
}

void CalendarWidget::setupConnection() {
    calendarGroupBox->setTitle(connectionManager->getCalendarName());
    QObject::connect(connectionManager, &ConnectionManager::calendarReady, this,
                     &CalendarWidget::onCalendarReady); //Connect
    getCalendarRequest();
}

void CalendarWidget::setupTimer() {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CalendarWidget::onTimeout);
    QObject::connect(connectionManager, &ConnectionManager::ctagChanged, this,
                     &CalendarWidget::getCalendarRequest); //Connect
    timer->start(timerInterval);
}

void CalendarWidget::onTaskDeleted(CalendarObject &obj) {
    timer->stop();
    connectionManager->getctag();
}

void CalendarWidget::onTimeout() {
    connectionManager->getctag();
}

QDate CalendarWidget::getCurrentDateSelected() {
    return currentDateEdit->date();
}
