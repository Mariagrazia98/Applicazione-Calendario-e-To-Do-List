#include "calendar.h"
#include "ui_calendar.h"

Calendar::Calendar(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::Calendar),
        dateString(new QTextBrowser),
        answerString(new QTextBrowser) {
    ui->setupUi(this);

    createCalendarGroupBox();

    setupCalendar();

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(calendarGroupBox, 0, 0);
    layout->addWidget(tasksGroupBox, 0, 1);

    layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);

    calendarLayout->setRowMinimumHeight(0, calendar->sizeHint().height());
    calendarLayout->setColumnMinimumWidth(0, calendar->sizeHint().width());

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

    QSize size = dateString->document()->size().toSize();
    dateString->setFixedHeight( 30 );
    dateString->setAlignment(Qt::AlignCenter);

    tasksGroupBox->setLayout(tasksLayout);
    tasksLayout->addWidget(answerString);

    addTaskButton = new QPushButton(tr("&Add"));
    addTaskButton->setEnabled(true);
    addTaskButton->setToolTip(tr("Add new task"));
    connect(addTaskButton, &QPushButton::clicked, this, &Calendar::addTaskButtonClicked);
    tasksLayout->addWidget(addTaskButton);
}

void Calendar::selectedDateChanged() {
    currentDateEdit->setDate(calendar->selectedDate());
    QDate date = currentDateEdit->date();
    QLocale locale = QLocale(QLocale::Italian, QLocale::Italy); // set the locale you want here
    QString italianDate = locale.toString(date, "dddd, d MMMM yyyy");
    dateString->setText(italianDate);
    dateString->setAlignment(Qt::AlignCenter);
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
    calendarGroupBox = new QGroupBox(tr("Preview"));

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
    answerString->setText(calendar);
}

void Calendar::addTaskButtonClicked() {
    // disabilita calendar widget
    this->setEnabled(false);
    TaskForm* taskForm = new TaskForm();
    // crea widget per aggiungere un nuovo task/evento/ecc...
    // widget.show()
    taskForm->show();
    // all'invio del form crea la query
    // chiude il widget
    // riabilita il calendar widget
    this->setEnabled(true);
    // aggiorna il calendario se la richiesta va a buon fine
}


