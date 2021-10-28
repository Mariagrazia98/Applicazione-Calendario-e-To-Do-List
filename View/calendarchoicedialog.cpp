//
// Created by manue on 04/10/2021.
//

// You may need to build the project (run Qt uic code generator) to get "ui_CalendarChoiceDialog.h" resolved

#include "calendarchoicedialog.h"
#include "ui_CalendarChoiceDialog.h"
#include "calendarwidget.h"


CalendarChoiceDialog::CalendarChoiceDialog(QWidget *parent, ConnectionManager *connectionManager) :
        QDialog(parent),
        connectionManager(std::make_shared<ConnectionManager *>(connectionManager)),
        ui(new Ui::CalendarChoiceDialog),
        formLayout(new QFormLayout),
        verticalLayout(new QVBoxLayout),
        pushButton(new QPushButton("Confirm")),
        calendarSelected(0) {
    ui->setupUi(this);
    pushButton->setEnabled(false);
}

CalendarChoiceDialog::~CalendarChoiceDialog() {
    delete ui;
}

void CalendarChoiceDialog::setConnectionManager(ConnectionManager *connectionManager) {
    this->connectionManager = std::make_shared<ConnectionManager *>(connectionManager);
}

void CalendarChoiceDialog::setupUI(QList<Calendar *> calendarsList) {
    this->calendarsList = calendarsList;
    for (int i = 0; i < calendarsList.length(); ++i) {
        Calendar *calendar = calendarsList[i];
        if (calendar) {
            QCheckBox *checkBox = new QCheckBox();
            connect(checkBox, &QCheckBox::stateChanged, this, &CalendarChoiceDialog::checkBoxToggled);
            checkBoxes.append(checkBox);
            QLabel *label = new QLabel(calendar->getName());
            formLayout->addRow(label, checkBox);
            /*
            QPushButton * button = new QPushButton(calendar->getName(), this);
            groupButton->addButton(button);
            verticalLayout->addWidget(button, 0, Qt::AlignCenter);
             */
        } else {
            std::cerr << "[CalendarChoiceDialog] null Calendar\n";
        }
    }
    formLayout->setAlignment(Qt::AlignCenter);
    verticalLayout->addLayout(formLayout);
    verticalLayout->addWidget(pushButton);
    connect(pushButton, &QPushButton::pressed,
            this, &CalendarChoiceDialog::onButtonClicked);

    verticalLayout->addSpacing(30);

    this->adjustSize();
    //this->setMinimumSize(400,300);

    setLayout(verticalLayout);
}

void CalendarChoiceDialog::onButtonClicked() {
    for (int i = 0; i < calendarsList.length(); ++i) {
        if (checkBoxes[i]->isChecked()) {
            if (calendarsList[i]) {
                std::cout << "Calendario " << calendarsList[i]->getName().toStdString() << " selezionato\n";
            }
        }
        /*
        if (calendarsList[i]->getName() == buttonText) {
            (*connectionManager.get())->setCalendar(calendarsList[i]);
            break;
        }
         */
    }
    this->accept();
}

void CalendarChoiceDialog::checkBoxToggled(int state) {
    switch (state) {
        case 0:
            calendarSelected--;
            if (calendarSelected == 0) {
                pushButton->setEnabled(false);
            }
            break;
        case 2:
            calendarSelected++;
            pushButton->setEnabled(true);
            break;
    }
}