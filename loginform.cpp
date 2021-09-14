#include "loginform.h"
#include "ui_loginform.h"

#include <QMessageBox>

LoginForm::LoginForm(QWidget *parent) :
        loggedIn(false),
        QDialog(parent),
        networkAccessManager(new QNetworkAccessManager),
        ui(new Ui::LoginForm) {
    ui->setupUi(this);
    setupRequest();
}

LoginForm::~LoginForm() {
    delete ui;
}

void LoginForm::closeEvent(QCloseEvent *event) {
    if (!loggedIn) {
        QApplication::quit();
    }
}

void LoginForm::on_loginButton_clicked() {
    makeRequest();
}

void LoginForm::setupRequest() {
    connect(networkAccessManager, SIGNAL(finished(QNetworkReply * )), this, SLOT(responseHandler(QNetworkReply * )));
    connect(networkAccessManager, SIGNAL(authenticationRequired(QNetworkReply * , QAuthenticator * )), this,
            SLOT(authenticationRequired(QNetworkReply * , QAuthenticator * )));
}

void LoginForm::makeRequest() {
    QNetworkRequest request;
    request.setUrl(QUrl("http://localhost/progettopds/calendarserver.php/calendars/admin/default?export"));
    networkAccessManager->get(request);
}

void LoginForm::responseHandler(QNetworkReply *reply) {
    QByteArray answer = reply->readAll();
    QString answerString = QString::fromUtf8(answer);

    QNetworkReply::NetworkError error = reply->error();
    const QString &errorString = reply->errorString();
    if (error != QNetworkReply::NoError) {
        QMessageBox::warning(this, "Error", errorString);
    } else {
        //QMessageBox::information(this, "Login", "Username and password is correct");
        loggedIn = true;
        accept();
        emit(calendarObtained(answerString));
        this->close();
    }
}

void LoginForm::authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator) {
    QString username = ui->usernameText->text();
    QString password = ui->passwordText->text();
    authenticator->setUser(username);
    authenticator->setPassword(password);
}