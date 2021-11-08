#include "loginform.h"

#include <QMessageBox>
#include <iostream>

LoginForm::LoginForm(QWidget *parent, std::shared_ptr<ConnectionManager> connectionManager) :
        QDialog(parent),
        connectionManager(connectionManager),
        groupBox(new QGroupBox),
        formLayout(new QFormLayout),
        layout(new QGridLayout),
        userLabel(new QLabel("Username")),
        user(new QLineEdit),
        passwordLabel(new QLabel("Password")),
        password(new QLineEdit),
        dialogButtonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close)){

    formLayout->addRow(userLabel, user);
    formLayout->addRow(passwordLabel, password);
    password->setEchoMode(QLineEdit::Password);
    formLayout->addWidget(dialogButtonBox);

    groupBox->setLayout(formLayout);
    layout->addWidget(groupBox);
    layout->setAlignment(Qt::AlignCenter);
    setLayout(layout);
    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &LoginForm::onLoginButtonClicked);
    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &LoginForm::close);
}

void LoginForm::onLoginButtonClicked() {
    dialogButtonBox->setDisabled(true);
    // set connection manager attributes
    connectionManager->setUsername(user->text());
    connectionManager->setPassword(password->text());
    // get the list of all calendars
    connectionManager->getCalendarList();
    connect(connectionManager.get(), &ConnectionManager::loggedin, this, &LoginForm::responseHandler);
}

void LoginForm::responseHandler(QNetworkReply *reply) {
    disconnect(connectionManager.get(), &ConnectionManager::loggedin, this, &LoginForm::responseHandler);
    QByteArray answer = reply->readAll();
    QString answerString = QString::fromUtf8(answer);
    QNetworkReply::NetworkError error = reply->error();
    const QString &errorString = reply->errorString();
    if (error != QNetworkReply::NoError) {
        QMessageBox::warning(this, "Error", errorString);
    } else {
        accept();
    }
    dialogButtonBox->setDisabled(false);
    reply->deleteLater();
}

