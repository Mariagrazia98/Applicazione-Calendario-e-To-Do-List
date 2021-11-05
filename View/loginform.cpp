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
        calendarLabel(new QLabel("Calendar")),
        calendar(new QLineEdit),
        dialogButtonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close)),
        ui(new Ui::LoginForm),
        networkAccessManager(new QNetworkAccessManager) {
    ui->setupUi(this);

    calendar->setText("default");
    formLayout->addRow(userLabel, user);
    formLayout->addRow(passwordLabel, password);
    password->setEchoMode(QLineEdit::Password);
    formLayout->addRow(calendarLabel, calendar);
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
    connectionManager->setUsername(user->text());
    connectionManager->setPassword(password->text());
    connectionManager->setCalendar(calendar->text());

    //QString authorization = "Basic ";
    //authorization.append((connectionManager->getUsername() + ":" + connectionManager->getPassword()));

    QBuffer *buffer = new QBuffer();

    buffer->open(QIODevice::ReadWrite);

    QString requestString =
            "<d:propfind xmlns:d=\"DAV:\" xmlns:cs=\"http://calendarserver.org/ns/\">\r\n"
            "  <d:prop>\r\n"
            "    <d:displayname />\r\n"
            "    <cs:getctag />\r\n"
            "    <d:sync-token />"
            "  </d:prop>\r\n"
            "</d:propfind>";

    int buffersize = buffer->write(requestString.toUtf8());
    buffer->seek(0);
    buffer->size();

    QByteArray contentlength;
    contentlength.append(buffersize);

    QString authorization = "Basic ";
    authorization.append(
            (connectionManager->getUsername() + ":" + connectionManager->getPassword()).toUtf8().toBase64());

    QNetworkRequest request;
    request.setUrl(connectionManager->getServerUrl());
    request.setRawHeader("User-Agent", "CalendarClient_CalDAV");
    request.setRawHeader("Authorization", authorization.toUtf8());
    request.setRawHeader("Depth", "0");
    request.setRawHeader("Prefer", "return-minimal");
    request.setRawHeader("Content-Type", "text/xml; charset=utf-8");
    request.setRawHeader("Content-Length", contentlength);

    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(conf);

    QNetworkReply *networkReply = networkAccessManager->sendCustomRequest(request, QByteArray("PROPFIND"), buffer);

    if (NULL != networkReply) {
        //connect(networkReply, &QNetworkReply::errorOccurred, this, &LoginForm::onErrorOccurred);

        connection = connect(networkAccessManager, &QNetworkAccessManager::finished, this, &LoginForm::responseHandler);
        connect(networkAccessManager, &QNetworkAccessManager::authenticationRequired, connectionManager, &ConnectionManager::authenticationRequired);
        //m_RequestTimeoutTimer.start(m_RequestTimeoutMS);
    } else {
        std::cerr << "ERROR: Invalid reply pointer when requesting sync token.";
    }

}

void LoginForm::setConnectionManager(ConnectionManager *connectionManager) {
    this->connectionManager = connectionManager;
}

void LoginForm::responseHandler(QNetworkReply *reply) {
    QByteArray answer = reply->readAll();
    QString answerString = QString::fromUtf8(answer);
    //disconnect(connection);
    QNetworkReply::NetworkError error = reply->error();
    const QString &errorString = reply->errorString();
    //std::cout << errorString.toStdString() << std::endl;
    std::cout << answerString.toStdString() << '\n';
    if (error != QNetworkReply::NoError) {
        QMessageBox::warning(this, "Error", errorString);
    } else {
        //QMessageBox::information(this, "Login", "Username and password is correct");
        accept();
        this->close();
    }
}

void LoginForm::onErrorOccurred(QNetworkReply::NetworkError code) {
    // TODO: error handling
    switch (code) {
        case QNetworkReply::ConnectionRefusedError:
            std::cerr << "Connection Refused\n";
            break;
        default:
            break;
    }
    std::cerr << "Error code: " << code << '\n';
}

