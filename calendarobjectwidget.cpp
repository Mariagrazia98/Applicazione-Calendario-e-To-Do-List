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
        modifyButton(new QPushButton(this)),
        removeButton(new QPushButton(this)),
        ui(new Ui::calendarObjectWidget) {
    ui->setupUi(this);
    setupUI();
}

calendarObjectWidget::~calendarObjectWidget() {
    delete ui;
}


void calendarObjectWidget::setupUI() {
    checkBox->setVisible(false); // TODO: set visible if To-Do
    setupText();
    displayLayout->addWidget(textBrowser);
    setupButtons();
    this->setLayout(displayLayout);
}

void calendarObjectWidget::setupText() {
    QString text;
    text.append("Name: " + calendarObject->getName() + '\n');
    text.append("Description: " + calendarObject->getDescription() + '\n');
    text.append("Location: " + calendarObject->getLocation() + '\n');
    CalendarEvent *calendarEvent = static_cast<CalendarEvent *>(calendarObject);
    if (calendarEvent != nullptr) {
        // calendarObject is a CalendarEvent
        QLocale locale = QLocale(QLocale::Italian, QLocale::Italy); // TODO: impostare in inglese ?
        text.append("StartDateTime: " + locale.toString(calendarEvent->getStartDateTime(), "dddd, d MMMM yyyy") + '\n');
        text.append("EndDateTime: " + locale.toString(calendarEvent->getEndDateTime(), "dddd, d MMMM yyyy") + '\n');
    }
    textBrowser->setText(text);
}


void calendarObjectWidget::setupButtons() {
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
}
