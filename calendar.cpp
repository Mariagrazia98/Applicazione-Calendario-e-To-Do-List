#include "calendar.h"
#include "ui_calendar.h"

Calendar::Calendar(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::Calendar),
        networkAccessManager(new QNetworkAccessManager),
        dateString(new QTextBrowser),
        answerString(new QTextBrowser) {
    ui->setupUi(this);

    createPreviewGroupBox();
    createGeneralOptionsGroupBox();
    //createDatesGroupBox();
    //createTextFormatsGroupBox();

    setupCalendar();

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(previewGroupBox, 0, 0);
    layout->addWidget(generalOptionsGroupBox, 0, 1);
    //layout->addWidget(datesGroupBox, 1, 0);
    //layout->addWidget(textFormatsGroupBox, 1, 1);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);

    previewLayout->setRowMinimumHeight(0, calendar->sizeHint().height());
    previewLayout->setColumnMinimumWidth(0, calendar->sizeHint().width());

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
    generalOptionsGroupBox = new QGroupBox(tr("Tasks"));

    tasksLayout = new QVBoxLayout;

    QDate date = currentDateEdit->date();
    QLocale locale = QLocale(QLocale::Italian, QLocale::Italy); // set the locale you want here
    QString italianDate = locale.toString(date, "dddd, d MMMM yyyy");

    dateString->setText(italianDate);
    tasksLayout->addWidget(dateString);

    QSize size = dateString->document()->size().toSize();
    dateString->setFixedHeight( 30 );
    dateString->setAlignment(Qt::AlignCenter);

    // TODO: aggiungere widget tasks
    setupRequest();

    generalOptionsGroupBox->setLayout(tasksLayout);
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

void Calendar::createPreviewGroupBox() {
    previewGroupBox = new QGroupBox(tr("Preview"));

    calendar = new QCalendarWidget;
    calendar->setMinimumDate(QDate(2000, 1, 1));
    calendar->setMaximumDate(QDate(2121, 12, 31));
    calendar->setGridVisible(true);

    connect(calendar, &QCalendarWidget::currentPageChanged,
            this, &Calendar::reformatCalendarPage);

    previewLayout = new QGridLayout;
    previewLayout->addWidget(calendar, 0, 0, Qt::AlignCenter);
    previewGroupBox->setLayout(previewLayout);
}

void Calendar::createGeneralOptionsGroupBox() {
    //TODO use as template for tasks widget



}

QComboBox *Calendar::createColorComboBox() {
    QComboBox *comboBox = new QComboBox;
    comboBox->addItem(tr("Red"), QColor(Qt::red));
    comboBox->addItem(tr("Blue"), QColor(Qt::blue));
    comboBox->addItem(tr("Black"), QColor(Qt::black));
    comboBox->addItem(tr("Magenta"), QColor(Qt::magenta));
    return comboBox;
}

void Calendar::setupRequest() {
    QNetworkRequest request;
    request.setUrl(QUrl("http://localhost/progettopds/calendarserver.php/calendars/admin/default?export"));

    connect(networkAccessManager, SIGNAL(finished(QNetworkReply * )), this, SLOT(responseHandler(QNetworkReply * )));
    connect(networkAccessManager, SIGNAL(authenticationRequired(QNetworkReply * , QAuthenticator * )), this,
            SLOT(authenticationRequired(QNetworkReply * , QAuthenticator * )));

    networkAccessManager->get(request);
}

void Calendar::responseHandler(QNetworkReply *serverAnswer) {
    QByteArray answer = serverAnswer->readAll();
    answerString->setText(QString::fromUtf8(answer));
    tasksLayout->addWidget(answerString);
}

void Calendar::authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator) {
    authenticator->setPassword(QString("admin"));
    authenticator->setUser(QString("admin"));
}

void Calendar::onDateTextChanged() {

}


