#include "calendar.h"
#include "ui_calendar.h"

Calendar::Calendar(QWidget *parent, ConnectionManager *connectionManager) :
        QWidget(parent),
        ui(new Ui::Calendar),
        dateString(new QTextBrowser),
        connectionManager(connectionManager),
        stream(new QTextStream()) {
    ui->setupUi(this);
    //connect(connectionManager, &ConnectionManager::finished, this, &Calendar::finished);

    createCalendarGroupBox();

    setupCalendar();

    QGridLayout * layout = new QGridLayout;
    layout->addWidget(calendarGroupBox, 0, 0);
    layout->addWidget(tasksGroupBox, 0, 1);

    //layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);

    layout->setRowMinimumHeight(0, calendar->sizeHint().height());
    layout->setColumnMinimumWidth(0, calendar->sizeHint().width());
    layout->setRowMinimumHeight(0, calendar->sizeHint().height());
    layout->setColumnMinimumWidth(1, calendar->sizeHint().width() * 1.5);

    setMinimumHeight(480);

    setWindowTitle(tr("Calendar Widget"));
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
    currentDateEdit->setDisplayFormat("MMM d yyyy");
    currentDateEdit->setDate(QDate::currentDate());
    currentDateEdit->setDateRange(calendar->minimumDate(),
                                  calendar->maximumDate());

    setupWeek();
    connect(calendar, &QCalendarWidget::selectionChanged,
            this, &Calendar::selectedDateChanged);
    tasksGroupBox = new QGroupBox(tr("Tasks"));

    tasksLayout = new QVBoxLayout;

    QDate date = currentDateEdit->date();
    QLocale locale = QLocale(QLocale::Italian, QLocale::Italy); // set the locale you want here
    QString italianDate = locale.toString(date, "dddd, d MMMM yyyy");

    dateString->setText(italianDate);
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
        QLocale locale = QLocale(QLocale::Italian, QLocale::Italy); // set the locale you want here
        QString italianDate = locale.toString(date, "dddd, d MMMM yyyy");
        dateString->setText(italianDate);
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
        }
    }
    stream->seek(0);

    showSelectedDateTasks();
}

void Calendar::showSelectedDateTasks() {
    QLayoutItem *item;

    while ((item = taskViewLayout->layout()->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    for (int i = 0; i < calendarObjects.length(); ++i) {
        CalendarEvent *calendarEvent = static_cast<CalendarEvent *>(calendarObjects[i]);
        if (calendarEvent && calendarEvent->getStartDateTime().date() <= calendar->selectedDate() &&
            calendarEvent->getEndDateTime().date() >= calendar->selectedDate()) {
            CalendarObjectWidget *obj = new CalendarObjectWidget(this, *calendarObjects[i], connectionManager);
            obj->setVisible(true);
            obj->setEnabled(true);
            taskViewLayout->addWidget(obj);
            connect(obj, &CalendarObjectWidget::taskModified, this, &Calendar::onTaskModified);
            connect(obj, &CalendarObjectWidget::taskDeleted, this, &Calendar::onTaskDeleted);
        }
    }
}


void Calendar::parseEvent() {
    QString line;
    CalendarObject *calendarObject = new CalendarEvent(this);
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
            static_cast<CalendarEvent *>(calendarObject)->setCreationDateTime(
                    getDateTimeFromString(value).toLocalTime());
        } else if (key.startsWith(QLatin1String("DTSTART"))) {
            static_cast<CalendarEvent *>(calendarObject)->setStartDateTime(getDateTimeFromString(value).toLocalTime());
        } else if (key.startsWith(QLatin1String("DTEND"))) {
            static_cast<CalendarEvent *>(calendarObject)->setEndDateTime(getDateTimeFromString(value).toLocalTime());
        } else if (key == QLatin1String("SUMMARY")) {
            calendarObject->setName(value);
        } else if (key == QLatin1String("LOCATION")) {
            calendarObject->setLocation(value);
        } else if (key == QLatin1String("UID")) {
            calendarObject->setUID(value);
        } else if (key == QLatin1String("DESCRIPTION")) {
            calendarObject->setDescription(value);
        }
    }
}

void Calendar::addTaskButtonClicked() {
    addTaskButton->setEnabled(false);
    TaskForm *taskForm = new TaskForm();
    taskForm->show();
    connect(taskForm, &TaskForm::closing, this, &Calendar::onTaskFormClosed);
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
        qDebug() << "Calendar" << ": " << "could not parse" << string;
    return dateTime;
}

void Calendar::onTaskFormClosed() {
    addTaskButton->setEnabled(true);
    // TODO: aggiornare widgets se ce n'è bisogno
}

void Calendar::onTaskModified(CalendarObject &obj) {
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
        std::cout << "parse calendar after clear\n";
        parseCalendar(answerString);
    }
}

void Calendar::getCalendarRequest() {
    connectionManager->getCalendarRequest();
}

void Calendar::setupConnection() {
    connectionToFinished = QObject::connect(connectionManager, &ConnectionManager::finished, this, &Calendar::finished); //Connect
    getCalendarRequest();
}

void Calendar::onTaskDeleted(CalendarObject &obj) {
    std::cout<<"on task deleted\n";
    setupConnection();
}



