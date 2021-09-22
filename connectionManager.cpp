//
// Created by mary_ on 17/09/2021.
//

#include <iostream>

#include "connectionManager.h"

ConnectionManager::ConnectionManager(QString username, QString password, QString calendar) :
        networkAccessManager(new QNetworkAccessManager),
        username(username),
        password(password),
        calendar(calendar) {
    updateUrl();
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

    emit(finished(reply));

}

void ConnectionManager::setUsername(QString username) {
    this->username = username;
    updateUrl();
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

void ConnectionManager::addOrUpdateCalendarObject(const QString &requestString, const QString &UID) {
    QBuffer *buffer = new QBuffer();

    buffer->open(QIODevice::ReadWrite);
    int buffersize = buffer->write(requestString.toUtf8());
    buffer->seek(0);
    buffer->size();

    QByteArray contentlength;
    contentlength.append(buffersize);

    QNetworkRequest request;
    QString filename = UID + ".ics";
    request.setUrl(QUrl(serverUrl.toString() + '/' + filename));

    request.setRawHeader("Content-Type", "text/calendar; charset=utf-8");
    request.setRawHeader("Content-Length", contentlength);

    QNetworkReply *networkReply = networkAccessManager->put(request, buffer);

    if (networkReply) {
        /*connect(qNetworkReply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(handleUploadHTTPError())); */

        //m_UploadRequestTimeoutTimer.start(m_RequestTimeoutMS);
    } else {
        //QDEBUG << m_DisplayName << ": " << "ERROR: Invalid reply pointer when requesting URL.";
        //emit error("Invalid reply pointer when requesting URL.");
        std::cerr << "Invalid reply pointer when requesting URL\n";
    }
}

const QString &ConnectionManager::getCalendar() const {
    return calendar;
}

void ConnectionManager::setCalendar(const QString &calendar) {
    ConnectionManager::calendar = calendar;
    updateUrl();
}

void ConnectionManager::updateUrl() {
    serverUrl = QUrl("http://localhost/progettopds/calendarserver.php/calendars/" + username + '/' + calendar);
}

