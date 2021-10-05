//
// Created by manue on 04/10/2021.
//

// You may need to build the project (run Qt uic code generator) to get "ui_CalendarChoiceDialog.h" resolved

#include "calendarchoicedialog.h"
#include "ui_CalendarChoiceDialog.h"


CalendarChoiceDialog::CalendarChoiceDialog(QWidget *parent, ConnectionManager *connectionManager) :
        QDialog(parent),
        connectionManager(connectionManager),
        ui(new Ui::CalendarChoiceDialog),
        layout(new QGridLayout),
        buttonsLayout(new QVBoxLayout),
        groupButton(new QButtonGroup) {
    ui->setupUi(this);

}

CalendarChoiceDialog::~CalendarChoiceDialog() {
    delete ui;
}

void CalendarChoiceDialog::setConnectionManager(ConnectionManager *connectionManager) {
    this->connectionManager = connectionManager;
}

void CalendarChoiceDialog::setupUI(QList<Calendar *> calendarsList) {
    for (int i = 0; i < calendarsList.length(); ++i) {
        Calendar *calendar = calendarsList[i];
        if (calendar) {
            QPushButton *button = new QPushButton(calendar->getName(), this);
            groupButton->addButton(button);
            buttonsLayout->addWidget(button, 0, Qt::AlignCenter);
        } else {
            std::cerr << "[CalendarChoiceDialog] null Calendar\n";
        }
    }
    connect(groupButton, SIGNAL(buttonPressed(QAbstractButton * )),
            this, SLOT(onGroupButtonClicked(QAbstractButton * )));
    setLayout(buttonsLayout);
}

void CalendarChoiceDialog::onGroupButtonClicked(QAbstractButton *button) {
    connectionManager->setCalendar(button->text());
    connectionManager->tryLogin();
}
