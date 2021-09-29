#include "calendar.h"
#include "ui_calendar.h"

#define DAILY 1
#define WEEKLY 2
#define MONTHLY 3
#define YEARLY 4

Calendar::Calendar(QWidget *parent, ConnectionManager *connectionManager) :
        QWidget(parent),
        ui(new Ui::Calendar),
        dateString(new QTextBrowser),
        connectionManager(connectionManager),
        stream(new QTextStream()) {
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
    layout->setRowMinimumHeight(0, calendar->sizeHint().height());
    layout->setColumnMinimumWidth(1, calendar->sizeHint().width() * 1.5);

    setMinimumHeight(480);

    setWindowTitle(tr("Calendar Application"));
}

Calendar::~Calendar() {
    delete ui;
}

void Calendar::setupCalendar() {
    // Calendar setup
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
            this, &Calendar::selectedDateChanged);
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
    connect(addTaskButton, &QPushButton::clicked, this, &Calendar::addTaskButtonClicked);
    tasksLayout->addWidget(addTaskButton);
}

void Calendar::selectedDateChanged() {
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

void Calendar::setupWeek() {
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

void Calendar::reformatCalendarPage() {
    QTextCharFormat mayFirstFormat;
    const QDate mayFirst(calendar->yearShown(), 5, 1);

    QTextCharFormat firstFridayFormat;
    QDate firstFriday(calendar->yearShown(), calendar->monthShown(), 1);
    while (firstFriday.dayOfWeek() != Qt::Friday)
        firstFriday = firstFriday.addDays(1);

    calendar->setDateTextFormat(firstFriday, firstFridayFormat);
    calendar->setDateTextFormat(mayFirst, mayFirstFormat);
}

void Calendar::createCalendarGroupBox() {
    calendarGroupBox = new QGroupBox(tr("Calendar"));

    calendar = new QCalendarWidget;
    calendar->setLocale(QLocale::English);
    calendar->setMinimumDate(QDate(2000, 1, 1));
    calendar->setMaximumDate(QDate(2121, 12, 31));
    calendar->setGridVisible(true);

    connect(calendar, &QCalendarWidget::currentPageChanged,
            this, &Calendar::reformatCalendarPage);

    calendarLayout = new QGridLayout;
    calendarLayout->addWidget(calendar);
    calendarGroupBox->setLayout(calendarLayout);
}

QComboBox *Calendar::createColorComboBox() {
    QComboBox *comboBox = new QComboBox;
    comboBox->addItem(tr("Red"), QColor(Qt::red));
    comboBox->addItem(tr("Blue"), QColor(Qt::blue));
    comboBox->addItem(tr("Black"), QColor(Qt::black));
    comboBox->addItem(tr("Magenta"), QColor(Qt::magenta));
    return comboBox;
}

void Calendar::onDateTextChanged() {

}

void Calendar::parseCalendar(QString calendar) {
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

    showSelectedDateTasks();
}

void Calendar::showSelectedDateTasks() {
    QLayoutItem *item;
    while ((item = taskViewLayout->layout()->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    for (int i = 0; i < calendarObjects.length(); i++) {
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
                                addCalendarObjectWidget(calendarEvent_);
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
            if (calendarToDo->getStartDateTime()) {
                start = *calendarToDo->getStartDateTime();
            } else {
                start = calendarToDo->getCreationDateTime();
            }
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
                                    calendarToDo_->setCompletedDateTime(std::nullopt);
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
                                    calendarToDo_->setCompletedDateTime(std::nullopt);
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
                                    calendarToDo_->setCompletedDateTime(std::nullopt);
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
                                    calendarToDo_->setCompletedDateTime(std::nullopt);
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

void Calendar::addCalendarObjectWidget(CalendarObject *calendarObject) {
    CalendarObjectWidget *calendarObjectWidget = new CalendarObjectWidget(this, *calendarObject, connectionManager);
    calendarObjectWidget->setVisible(true);
    calendarObjectWidget->setEnabled(true);
    taskViewLayout->addWidget(calendarObjectWidget);
    connect(calendarObjectWidget, &CalendarObjectWidget::taskModified, this, &Calendar::onTaskModified);
    connect(calendarObjectWidget, &CalendarObjectWidget::taskDeleted, this, &Calendar::onTaskDeleted);
}

void Calendar::parseEvent() {
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
        }
    }
}

void Calendar::parseToDo() {
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


void Calendar::addTaskButtonClicked() {
    addTaskButton->setEnabled(false);
    TaskForm *taskForm = new TaskForm(connectionManager);
    taskForm->show();
    connect(taskForm, &TaskForm::closing, this, &Calendar::onTaskFormClosed);
    connect(taskForm, &TaskForm::taskUploaded, this, &Calendar::onTaskModified);
}

QDateTime Calendar::getDateTimeFromString(const QString &string) {
    QDateTime dateTime = QDateTime::fromString(string, "yyyyMMdd'T'hhmmss'Z'");
    if (!dateTime.isValid())
        dateTime = QDateTime::fromString(string, "yyyyMMdd'T'hhmmss");
    if (!dateTime.isValid())
        dateTime = QDateTime::fromString(string, "yyyyMMddhhmmss");
    if (!dateTime.isValid())
        dateTime = QDateTime::fromString(string, "yyyyMMdd");
    if (!dateTime.isValid())
        std::cerr << "Calendar" << ": " << "could not parse" << string.toStdString() << '\n';
    return dateTime;
}

void Calendar::onTaskFormClosed() {
    addTaskButton->setEnabled(true);
    // TODO: aggiornare widgets se ce n'è bisogno
}

void Calendar::onTaskModified() {
    setupConnection();
}

void Calendar::setConnectionManager(ConnectionManager *connectionManager) {
    Calendar::connectionManager = connectionManager;
}

void Calendar::finished(QNetworkReply *reply) {
    disconnect(connectionToFinished); //DISCONNECT

    QByteArray answer = reply->readAll();
    QString answerString = QString::fromUtf8(answer);

    QNetworkReply::NetworkError error = reply->error();
    const QString &errorString = reply->errorString();
    if (error != QNetworkReply::NoError) {
        QMessageBox::warning(this, "Error", errorString);
    } else {
        calendarObjects.clear();

        parseCalendar(answerString);
    }


}

void Calendar::getCalendarRequest() {
    connectionManager->getCalendarRequest();
}

void Calendar::setupConnection() {
    connectionToFinished = QObject::connect(connectionManager, &ConnectionManager::finished, this,
                                            &Calendar::finished); //Connect
    getCalendarRequest();
}

void Calendar::setupTimer() {
    QTimer* timer = new QTimer(this);
    std::cout << "connection to timer\n";
    connect(timer, &QTimer::timeout, this, &Calendar::onTimeout);
    timer->start(10000);
}

void Calendar::onTaskDeleted(CalendarObject &obj) {
    setupConnection();
}

void Calendar::onTimeout() {
    std::cout << "timeout\n";
    connectionManager->getctag();
}


