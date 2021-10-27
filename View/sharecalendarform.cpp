//
// Created by manue on 26/10/2021.
//


#include "sharecalendarform.h"

ShareCalendarForm::ShareCalendarForm(QWidget *parent, ConnectionManager *connectionManager) :
        QDialog(parent),
        connectionManager(std::make_shared<ConnectionManager *>(connectionManager)),
        groupBox(new QGroupBox),
        formLayout(new QFormLayout),
        layout(new QGridLayout),
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

void ShareCalendarForm::onAcceptButtonClicked() {
    (*connectionManager.get())->makeShareCalendarRequest(email->text(), name->text(), comment->text());
    this->close();
}