//
// Created by mary_ on 17/09/2021.
//

#include <iostream>

#include "connectionManager.h"

ConnectionManager::ConnectionManager(QString username, QString password) :
        networkAccessManager(new QNetworkAccessManager),
        username(username),
        password(password),
        serverUrl(QUrl("http://localhost/progettopds/calendarserver.php/calendars/admin/default")) {
    setup();
}

void ConnectionManager::setup() {
    connect(networkAccessManager, SIGNAL(finished(QNetworkReply * )), this, SLOT(responseHandler(QNetworkReply * )));
    connect(networkAccessManager, SIGNAL(authenticationRequired(QNetworkReply * , QAuthenticator * )), this,
            SLOT(authenticationRequired(QNetworkReply * , QAuthenticator * )));
}

void ConnectionManager::getCalendarRequest() {
    QNetworkRequest request;
    request.setUrl(QUrl(serverUrl.toString() + "?export"));
    networkAccessManager->get(request);
}

void ConnectionManager::authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator) {
    authenticator->setUser(username);
    authenticator->setPassword(password);
}

void ConnectionManager::responseHandler(QNetworkReply *reply) {
    std::cout<<"Response handler finished\n";
    emit(finished(reply));
}

void ConnectionManager::setUsername(QString username) {
    this->username = username;
}

void ConnectionManager::setPassword(QString password) {
    this->password = password;
}

void ConnectionManager::deleteCalendarObject(const QString &UID) {
    if (UID.isEmpty()) {
        return;
    }
    QNetworkRequest request;
    request.setUrl(QUrl(serverUrl.toString() + "/" + UID + ".ics"));
    request.setRawHeader("Content-Type", "text/calendar; charset=utf-8");
    request.setRawHeader("Content-Length", 0);
    QNetworkReply *reply = networkAccessManager->deleteResource(request);
    if (!reply) {
        std::cerr << "something went wrong" << std::endl;
    } else {
        QMessageBox::information(nullptr, "ToDo", "Task deleted successfully");
    }
}

