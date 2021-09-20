#include "calendarobjectwidget.h"
#include "ui_calendarobjectwidget.h"

#include <iostream>

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
    if (calendarEvent) {
        checkBox->setVisible(false);
    } else {
        checkBox->setVisible(true);
    }
    displayLayout->addWidget(checkBox);
    setupText();
    displayLayout->addWidget(textBrowser);
    setupButtons();
    this->setLayout(displayLayout);
}

void CalendarObjectWidget::setupText() {
    QString text;
    text.append("Name: " + calendarObject->getName() + '\n');
    text.append("Description: " + calendarObject->getDescription() + '\n');
    text.append("Location: " + calendarObject->getLocation() + '\n');
    CalendarEvent *calendarEvent = dynamic_cast<CalendarEvent *>(calendarObject);
    if (calendarEvent != nullptr) {
        // calendarObject is a CalendarEvent
        QLocale locale = QLocale(QLocale::Italian, QLocale::Italy); // TODO: impostare in inglese ?
        text.append("StartDateTime: " + locale.toString(calendarEvent->getStartDateTime(), "dddd, d MMMM yyyy") + '\n');
        text.append("EndDateTime: " + locale.toString(calendarEvent->getEndDateTime(), "dddd, d MMMM yyyy") + '\n');
    }
    else{
        CalendarToDo *calendarToDo = dynamic_cast<CalendarToDo *>(calendarObject);
        if (calendarToDo != nullptr) {
            // calendarObject is a CalendarEvent
            QLocale locale = QLocale(QLocale::Italian, QLocale::Italy); // TODO: impostare in inglese ?
            if(calendarToDo->getStartDateTime())
            {
                text.append("StartDateTime: " + locale.toString(*calendarToDo->getStartDateTime(), "dddd, d MMMM yyyy") + '\n');
            }
            if(calendarToDo->getDueDateTime())
            {
                text.append("DueDateTime: " + locale.toString(*calendarToDo->getDueDateTime(), "dddd, d MMMM yyyy") + '\n');
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
    connect(taskForm, &TaskForm::taskUploaded, this, &CalendarObjectWidget::onTaskModified);
    //emit(taskModified(*calendarObject));

}

void CalendarObjectWidget::onRemoveButtonClicked() {

    connectionToFinish = connect(connectionManager, SIGNAL(finished(QNetworkReply * )), this,
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
        std::cout << error << "\n";
        QMessageBox::warning(this, "Error", errorString);
    } else {

        emit(taskDeleted(*calendarObject));
    }
}

void CalendarObjectWidget::onTaskModified() {
    emit(taskModified());
}


