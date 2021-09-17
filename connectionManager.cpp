//
// Created by mary_ on 17/09/2021.
//

#include <iostream>
#include "connectionManager.h"
ConnectionManager::ConnectionManager(QString username, QString password):
        networkAccessManager(new QNetworkAccessManager),
        username(username),
        password(password){
        setup();
}

void ConnectionManager::setup() {
    connect(networkAccessManager, SIGNAL(finished(QNetworkReply * )), this, SLOT(responseHandler(QNetworkReply * )));
    connect(networkAccessManager, SIGNAL(authenticationRequired(QNetworkReply * , QAuthenticator * )), this,
            SLOT(authenticationRequired(QNetworkReply * , QAuthenticator * )));
}

void ConnectionManager::getCalendarRequest() {
    QNetworkRequest request;
    request.setUrl(QUrl("http://localhost/progettopds/calendarserver.php/calendars/admin/default?export"));
    networkAccessManager->get(request);
}
void ConnectionManager::authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator) {
    authenticator->setUser(username);
    authenticator->setPassword(password);
}

void ConnectionManager::responseHandler(QNetworkReply *reply) {
    emit(finished(reply));
}

void ConnectionManager::setUsername(QString username) {
    this->username=username;
}

void ConnectionManager::setPassword(QString password) {
    this->password=password;
}

