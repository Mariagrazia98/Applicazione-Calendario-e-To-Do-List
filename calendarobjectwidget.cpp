#include "calendarobjectwidget.h"
#include "ui_calendarobjectwidget.h"

#include <iostream>

calendarObjectWidget::calendarObjectWidget(QWidget *parent, CalendarObject &calendarObject) :
        QWidget(parent),
        calendarObject(&calendarObject),
        displayLayout(new QHBoxLayout),
        buttonsLayout(new QVBoxLayout),
        checkBox(new QCheckBox),
        textBrowser(new QTextBrowser),
        modifyButton(new QPushButton("Modify", this)),
        removeButton(new QPushButton("Remove", this)),
        ui(new Ui::calendarObjectWidget) {
    ui->setupUi(this);
    calendarObject.printCalendarObject();
    setupUI();
}

calendarObjectWidget::~calendarObjectWidget() {
    delete ui;
}

void calendarObjectWidget::setupText() {
    QString text;
    text.append("Name: " + calendarObject->getName() + '\n');
    text.append("Description: " + calendarObject->getDescription() + '\n');
    text.append("Location: " + calendarObject->getLocation() + '\n');
    CalendarEvent *calendarEvent = static_cast<CalendarEvent *>(calendarObject);
    if (calendarEvent != nullptr) {
        // calendarObject is a CalendarEvent
        text.append("StartDateTime: " + calendarEvent->getStartDateTime().toString("yyyyMMddhhmmss") + '\n');
        text.append("EndDateTime: " + calendarEvent->getEndDateTime().toString("yyyyMMddhhmmss") + '\n');
    }
    std::cout << "------\n";
    std::cout << text.toStdString() << '\n';
    textBrowser->setText(text);
}

void calendarObjectWidget::setupUI() {
    checkBox->setVisible(false); // TODO: set visible if To-Do
    setupText();
    displayLayout->addWidget(textBrowser);
    buttonsLayout->addWidget(modifyButton);
    buttonsLayout->addWidget(removeButton);
    displayLayout->addLayout(buttonsLayout);
    this->setLayout(displayLayout);
}
