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
    connect(networkAccessManager, &QNetworkAccessManager::finished, this, &ConnectionManager::responseHandler);
    connect(networkAccessManager, &QNetworkAccessManager::authenticationRequired, this,
            &ConnectionManager::authenticationRequired);
}

void ConnectionManager::getCalendarRequest() {
    QNetworkRequest networkRequest;

    networkRequest.setUrl(QUrl(serverUrl.toString() + "?export"));
    networkAccessManager->get(networkRequest);
}

void ConnectionManager::authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator) {
    authenticator->setUser(username);
    authenticator->setPassword(password);
}

void ConnectionManager::responseHandler(QNetworkReply *reply) {
    emit(onFinished(reply));
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

    QNetworkRequest networkRequest;
    networkRequest.setUrl(QUrl(serverUrl.toString() + "/" + UID + ".ics"));
    networkRequest.setRawHeader("Content-Type", "text/calendar; charset=utf-8");
    networkRequest.setRawHeader("Content-Length", 0);

    QNetworkReply *networkReply = networkAccessManager->deleteResource(networkRequest);

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

    QNetworkRequest networkRequest;

    QString filename = UID + ".ics";
    networkRequest.setUrl(QUrl(serverUrl.toString() + '/' + filename));

    networkRequest.setRawHeader("Content-Type", "text/calendar; charset=utf-8");
    networkRequest.setRawHeader("Content-Length", contentlength);

    QNetworkReply *networkReply = networkAccessManager->put(networkRequest, buffer);

    if (networkReply) {
        /*connect(qNetworkReply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(handleUploadHTTPError())); */

        //m_UploadRequestTimeoutTimer.start(m_RequestTimeoutMS);
    } else {
        //QDEBUG << m_DisplayName << ": " << "ERROR: Invalid reply pointer when requesting URL.";
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
    makectagRequest();
    connectionToGetCtag = connect(networkAccessManager, &QNetworkAccessManager::finished, this,
                                  &ConnectionManager::checkctag);
}

void ConnectionManager::checkctag(QNetworkReply *reply) {
    disconnect(connectionToGetCtag);
    if (reply != nullptr) {
        QByteArray answer = reply->readAll();
        QString answerString = QString::fromUtf8(answer);
        QNetworkReply::NetworkError error = reply->error();
        const QString &errorString = reply->errorString();
        if (error == QNetworkReply::NoError) {
            parseAndUpdatectag(answerString);
        } else {
            // errore qui
            std::cerr << "checkctag: " << errorString.toStdString() << '\n';
        }
    }
}


void ConnectionManager::tryLogin() {
    makectagRequest();
    connectionToLogin = connect(networkAccessManager, &QNetworkAccessManager::finished, this, &ConnectionManager::onLoginRequestFinished);
}

void ConnectionManager::getUpdatedTasks() {

}

void ConnectionManager::parseAndUpdatectag(const QString &answerString) {
    const int startPosition = answerString.indexOf("<cs:getctag>");
    const int endPosition = answerString.indexOf("</cs:getctag>");
    QString ctagString = answerString.mid(startPosition, endPosition - startPosition);
    const int startctag = ctagString.lastIndexOf('/');
    ctagString = ctagString.mid(startctag + 1, -1);
    int new_ctag = ctagString.toInt();
    if (ctag != new_ctag && new_ctag > 0) { //something is changed
        ctag = new_ctag;
        std::cout << "new ctag: " << new_ctag << '\n';
        getCalendarRequest();
    }
}

void ConnectionManager::makectagRequest() {
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

    QNetworkRequest networkRequest;
    networkRequest.setUrl(serverUrl);
    networkRequest.setRawHeader("User-Agent", "CalendarClient_CalDAV");
    networkRequest.setRawHeader("Authorization", authorization.toUtf8());
    networkRequest.setRawHeader("Depth", "0");
    networkRequest.setRawHeader("Prefer", "return-minimal");
    networkRequest.setRawHeader("Content-Type", "text/xml; charset=utf-8");
    networkRequest.setRawHeader("Content-Length", contentlength);

    QSslConfiguration conf = networkRequest.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    networkRequest.setSslConfiguration(conf);

    QNetworkReply *networkReply = networkAccessManager->sendCustomRequest(networkRequest, QByteArray("PROPFIND"),
                                                                          buffer);
}

void ConnectionManager::onLoginRequestFinished(QNetworkReply *reply) {
    disconnect(connectionToLogin);
    QByteArray answer = reply->readAll();
    QString answerString = QString::fromUtf8(answer);
    QNetworkReply::NetworkError error = reply->error();
    const QString &errorString = reply->errorString();
    if (error == QNetworkReply::NoError) {
        parseAndUpdatectag(answerString);
        emit(loggedin(reply));
    } else {
        std::cerr << "checkctag: " << errorString.toStdString() << '\n';
    }
}

