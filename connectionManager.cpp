//
// Created by mary_ on 17/09/2021.
//

#include <iostream>

#include "connectionManager.h"

ConnectionManager::ConnectionManager(QString username, QString password, QString calendar) :
        networkAccessManager(new QNetworkAccessManager),
        username(username),
        password(password),
        calendar(calendar),
        ctag(-1) {
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
    networkReply = nullptr;
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

    networkReply = networkAccessManager->deleteResource(request);

    if (!networkReply) {
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

    networkReply = networkAccessManager->put(request, buffer);

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

void ConnectionManager::getctag() {
    std::cout << "getctag\n";
    if (networkReply != nullptr) {
        QByteArray answer = networkReply->readAll();
        QString answerString = QString::fromUtf8(answer);
        std::cout << "getctag aborted\n";
        return;
    }
    QBuffer *buffer = new QBuffer();

    buffer->open(QIODevice::ReadWrite);

    QString requestString =
            "<d:propfind xmlns:d=\"DAV:\" xmlns:cs=\"http://calendarserver.org/ns/\">\r\n"
            " <d:prop>\r\n"
            "    <d:displayname />\r\n"
            "    <cs:getctag />\r\n"
            "  </d:prop>\r\n"
            "</d:propfind>";

    int buffersize = buffer->write(requestString.toUtf8());
    buffer->seek(0);
    buffer->size();

    QByteArray contentlength;
    contentlength.append(buffersize);

    QString authorization = "Basic ";
    authorization.append(
            (username + ":" + password).toUtf8().toBase64());

    QNetworkRequest request;
    request.setUrl(serverUrl);
    request.setRawHeader("User-Agent", "CalendarClient_CalDAV");
    request.setRawHeader("Authorization", authorization.toUtf8());
    request.setRawHeader("Depth", "0");
    request.setRawHeader("Prefer", "return-minimal");
    request.setRawHeader("Content-Type", "text/xml; charset=utf-8");
    request.setRawHeader("Content-Length", contentlength);

    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(conf);

    networkReply = networkAccessManager->sendCustomRequest(request, QByteArray("PROPFIND"), buffer);

    if (networkReply) {

        connectionToGetCtag = connect(networkAccessManager, &QNetworkAccessManager::finished, this,
                                      &ConnectionManager::checkctag);

    } else {
        std::cerr << "ERROR: Invalid reply pointer when requesting sync token.";
    }
}

void ConnectionManager::checkctag(QNetworkReply *reply) {
    disconnect(connectionToGetCtag);
    if(reply != nullptr){
        QByteArray answer = reply->readAll();
        QString answerString = QString::fromUtf8(answer);
        std::cout << answerString.toStdString() << "\n\n";

        QNetworkReply::NetworkError error = reply->error();
        const QString &errorString = reply->errorString();
        if (error == QNetworkReply::NoError) {
            const int startPosition = answerString.indexOf("<cs:getctag>");
            const int endPosition = answerString.indexOf("</cs:getctag>");
            QString ctagString = answerString.mid(startPosition, endPosition - startPosition);
            const int startctag = ctagString.lastIndexOf('/');
            ctagString = ctagString.mid(startctag + 1, -1);
            std::cout << "ctag: " << ctagString.toStdString() << "\n";
            ctag = ctagString.toInt();
        } else {
            std::cerr << "checkctag: "<< errorString.toStdString() << '\n';
        }
    }

}

