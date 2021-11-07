//
// Created by manue on 04/10/2021.
//

// You may need to build the project (run Qt uic code generator) to get "ui_CalendarChoiceDialog.h" resolved

#include "calendarchoicedialog.h"
#include "ui_CalendarChoiceDialog.h"
#include "calendarwidget.h"


CalendarChoiceDialog::CalendarChoiceDialog(QWidget *parent, std::shared_ptr<ConnectionManager> connectionManager) :
        QDialog(parent),
        connectionManager(connectionManager),
        ui(new Ui::CalendarChoiceDialog),
        groupBox(new QGroupBox("Calendars")),
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

void CalendarChoiceDialog::setupUI(QList<Calendar *> calendarsList) {
    this->calendarsList = calendarsList;
    for (int i = 0; i < calendarsList.length(); ++i) {
        Calendar *calendar = calendarsList[i];
        if (calendar) {
            QCheckBox *checkBox = new QCheckBox();
            connect(checkBox, &QCheckBox::stateChanged, this, &CalendarChoiceDialog::checkBoxToggled);
            checkBoxes.append(checkBox);
            QLabel *label = new QLabel(calendar->getName());
            formLayout->addRow(checkBox, label);
        } else {
            std::cerr << "[CalendarChoiceDialog] null Calendar\n";
        }
    }
    formLayout->setAlignment(Qt::AlignCenter);
    groupBox->setAlignment(Qt::AlignCenter);
    groupBox->setLayout(formLayout);
    verticalLayout->addWidget(groupBox);
    verticalLayout->addWidget(pushButton);
    setLayout(verticalLayout);
    connect(pushButton, &QPushButton::pressed,
            this, &CalendarChoiceDialog::accept);

    //this->adjustSize();
    //this->setMinimumSize(400,300);

    setLayout(verticalLayout);
}

void CalendarChoiceDialog::checkBoxToggled(int state) {
    switch (state) {
        case 0:
            // calendar unchecked
            calendarSelected--;
            if (calendarSelected == 0) {
                // disable accept button if there are no calendars selected
                pushButton->setEnabled(false);
            }
            break;
        case 2:
            // calendar checked
            calendarSelected++;
            pushButton->setEnabled(true);
            break;
    }
}

QList<Calendar *> CalendarChoiceDialog::getSelectedCalendars() {
    QList<Calendar *> ret;
    for (int i = 0; i < calendarsList.length(); ++i) {
        if (checkBoxes[i]->isChecked()) {
            if (calendarsList[i]) {
                ret.append(calendarsList[i]);
            }
        }
    }
    return ret;
}
