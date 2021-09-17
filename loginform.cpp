#include "loginform.h"
#include "ui_loginform.h"

#include <QMessageBox>
#include <iostream>

LoginForm::LoginForm(QWidget *parent, ConnectionManager* connectionManager) :
        QDialog(parent),
        connectionManager(connectionManager),
        ui(new Ui::LoginForm) {
    ui->setupUi(this);
}

LoginForm::~LoginForm() {
    delete ui;
}

void LoginForm::on_loginButton_clicked() {
    connectionManager->setUsername(ui->usernameText->text());
    connectionManager->setPassword(ui->passwordText->text());
    connectionManager->getCalendarRequest();
    connection = connect(connectionManager, &ConnectionManager::finished, this, &LoginForm::responseHandler);
}

void LoginForm::setConnectionManager(ConnectionManager *connectionManager) {
    this->connectionManager = connectionManager;
}

void LoginForm::responseHandler(QNetworkReply *reply) {
    QByteArray answer = reply->readAll();
    QString answerString = QString::fromUtf8(answer);
    disconnect(connection);
    QNetworkReply::NetworkError error = reply->error();
    const QString &errorString = reply->errorString();
    if (error != QNetworkReply::NoError) {
        QMessageBox::warning(this, "Error", errorString);
    } else {
        //QMessageBox::information(this, "Login", "Username and password is correct");

        std::cout << "login done \n";
        accept();
        this->close();
    }
}

