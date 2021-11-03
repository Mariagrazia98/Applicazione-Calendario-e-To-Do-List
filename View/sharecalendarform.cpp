//
// Created by manue on 26/10/2021.
//


#include "sharecalendarform.h"

ShareCalendarForm::ShareCalendarForm(QWidget *parent,
                                     QMap<QString, std::shared_ptr<ConnectionManager>> connectionManagers) :
        QDialog(parent),
        parent(parent),
        connectionManagers(connectionManagers),
        groupBox(new QGroupBox),
        formLayout(new QFormLayout),
        layout(new QGridLayout),
        calendar(new QComboBox),
        calendarLabel(new QLabel("Calendar")),
        emailLabel(new QLabel("Email")),
        email(new QLineEdit),
        nameLabel(new QLabel("Name")),
        name(new QLineEdit),
        commentLabel(new QLabel("Comment")),
        comment(new QLineEdit),
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
    formLayout->addRow(commentLabel, comment);
    formLayout->addWidget(dialogButtonBox);

    groupBox->setLayout(formLayout);
    layout->addWidget(groupBox);
    layout->setAlignment(Qt::AlignCenter);
    this->setLayout(layout);
    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &ShareCalendarForm::onAcceptButtonClicked);
    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &ShareCalendarForm::close);
}

void ShareCalendarForm::closeEvent(QCloseEvent *event) {
    emit(closing());
}


void ShareCalendarForm::onAcceptButtonClicked() {
    connectionManagers[calendar->currentText()]->makeShareCalendarRequest(calendar->currentText(), email->text(), name->text(), comment->text());
    this->close();
}