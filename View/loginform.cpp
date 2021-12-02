#include "loginform.h"

#include <QMessageBox>
#include <iostream>
#include <utility>

LoginForm::LoginForm(QWidget *parent, std::shared_ptr<ConnectionManager> connectionManager) :
        QDialog(parent),
        connectionManager(std::move(connectionManager)),
        groupBox(new QGroupBox),
        formLayout(new QFormLayout),
        layout(new QGridLayout),
        userLabel(new QLabel("Username")),
        user(new QLineEdit),
        passwordLabel(new QLabel("Password")),
        password(new QLineEdit),
        dialogButtonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close)){

    /* Setup UI */

    formLayout->addRow(userLabel, user);
    formLayout->addRow(passwordLabel, password);
    password->setEchoMode(QLineEdit::Password);
    formLayout->addWidget(dialogButtonBox);

    groupBox->setLayout(formLayout);
    layout->addWidget(groupBox);
    layout->setAlignment(Qt::AlignCenter);
    setLayout(layout);

    /* Connects */
    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &LoginForm::onLoginButtonClicked);
    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &LoginForm::close);
}

void LoginForm::onLoginButtonClicked() {
    dialogButtonBox->setDisabled(true); // disable the button to prevent multiple events
    // set connection manager attributes
    connectionManager->setUsername(user->text());
    connectionManager->setPassword(password->text());
    // get the list of all calendars
    connectionManager->getCalendarList();
    connect(connectionManager.get(), &ConnectionManager::loggedin, this, &LoginForm::responseHandler);
}

void LoginForm::responseHandler(QNetworkReply *reply) {
    disconnect(connectionManager.get(), &ConnectionManager::loggedin, this, &LoginForm::responseHandler);
    //QByteArray answer = reply->readAll();
    //QString answerString = QString::fromUtf8(answer);
    QNetworkReply::NetworkError error = reply->error();
    if (error != QNetworkReply::NoError) {
        const QString &errorString = reply->errorString();
        QMessageBox::warning(this, "Error", errorString);
    } else {
        accept();
    }
    dialogButtonBox->setDisabled(false); // enable the button
    reply->deleteLater();
}

