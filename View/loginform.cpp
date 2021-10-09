#include "loginform.h"
#include "ui_loginform.h"

#include <QMessageBox>
#include <iostream>

LoginForm::LoginForm(QWidget *parent, ConnectionManager *connectionManager) :
        QDialog(parent),
        connectionManager(connectionManager),
        groupBox(new QGroupBox),
        formLayout(new QFormLayout),
        layout(new QGridLayout),
        userLabel(new QLabel("Username")),
        user(new QLineEdit),
        passwordLabel(new QLabel("Password")),
        password(new QLineEdit),

        dialogButtonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close)),
        ui(new Ui::LoginForm) {
    ui->setupUi(this);

    formLayout->addRow(userLabel, user);
    formLayout->addRow(passwordLabel, password);
    password->setEchoMode(QLineEdit::Password);
    formLayout->addWidget(dialogButtonBox);

    groupBox->setLayout(formLayout);
    layout->addWidget(groupBox);
    layout->setAlignment(Qt::AlignCenter);
    this->setLayout(layout);
    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &LoginForm::onLoginButtonClicked);
    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &LoginForm::close);
}

LoginForm::~LoginForm() {
    delete ui;
}

void LoginForm::onLoginButtonClicked() {
    dialogButtonBox->setDisabled(true);
    connectionManager->setUsername(user->text());
    connectionManager->setPassword(password->text());
    connectionManager->getCalendarList();
    connection = connect(connectionManager, &ConnectionManager::loggedin, this, &LoginForm::responseHandler);
}

void LoginForm::setConnectionManager(ConnectionManager *connectionManager) {
    this->connectionManager = connectionManager;
}


void LoginForm::responseHandler(QNetworkReply *reply) {
    disconnect(connection);
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
}

