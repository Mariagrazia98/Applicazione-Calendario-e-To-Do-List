//
// Created by manue on 26/10/2021.
//


#include "sharecalendarform.h"

#include <utility>


ShareCalendarForm::ShareCalendarForm(QWidget *parent,
                                     QMap<QString, std::shared_ptr<ConnectionManager>> connectionManagers) :
        QDialog(parent),
        parent(parent),
        connectionManagers(std::move(connectionManagers)),
        groupBox(new QGroupBox),
        formLayout(new QFormLayout),
        layout(new QGridLayout),
        calendar(new QComboBox),
        calendarLabel(new QLabel("Calendar")),
        emailLabel(new QLabel("Email")),
        email(new QLineEdit),
        nameLabel(new QLabel("Username")),
        name(new QLineEdit),
        dialogButtonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close)) {
    setupUI();
}


void ShareCalendarForm::setupUI() {
    for (auto connectionManager = connectionManagers.begin();
         connectionManager != connectionManagers.end(); connectionManager++) {
        calendar->addItem(connectionManager.key());
    }

    formLayout->addRow(calendarLabel, calendar);
    formLayout->addRow(emailLabel, email);
    formLayout->addRow(nameLabel, name);
    formLayout->addWidget(dialogButtonBox);

    groupBox->setLayout(formLayout);

    layout->addWidget(groupBox);
    layout->setAlignment(Qt::AlignCenter);
    this->setLayout(layout);
    resize(300,200);
    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &ShareCalendarForm::onAcceptButtonClicked);
    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &ShareCalendarForm::close);
}

void ShareCalendarForm::closeEvent(QCloseEvent *event) {
    /* emit signal */
    emit(shareCalendarFormClosed());
}


void ShareCalendarForm::onAcceptButtonClicked() {
    /* Control if the mail has been inserted. It is mandatory */
    if (email->text().isEmpty()) {
        QMessageBox::warning(this, "Error", "Insert an email");
        return;
    }
    /* Control if the name has been inserted. It is mandatory */
    if (name->text().isEmpty()){
    QMessageBox::warning(this, "Error", "Insert a username");
    return;
    }
    connectionManagers[calendar->currentText()]->makeShareCalendarRequest(calendar->currentText(), email->text(), name->text());
    this->close();
}