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
        networkRequest(nullptr),
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
    if (networkRequest == nullptr) {
        networkRequest = new QNetworkRequest();
    }
    networkRequest->setUrl(QUrl(serverUrl.toString() + "?export"));
    networkAccessManager->get(*networkRequest);
}

void ConnectionManager::authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator) {
    authenticator->setUser(username);
    authenticator->setPassword(password);
}

void ConnectionManager::responseHandler(QNetworkReply *reply) {
    networkRequest = nullptr;
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
    if (networkRequest == nullptr) {
        networkRequest = new QNetworkRequest();
    }
    networkRequest->setUrl(QUrl(serverUrl.toString() + "/" + UID + ".ics"));
    networkRequest->setRawHeader("Content-Type", "text/calendar; charset=utf-8");
    networkRequest->setRawHeader("Content-Length", 0);

    QNetworkReply *networkReply = networkAccessManager->deleteResource(*networkRequest);

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

    if (networkRequest == nullptr) {
        networkRequest = new QNetworkRequest();
    }

    QString filename = UID + ".ics";
    networkRequest->setUrl(QUrl(serverUrl.toString() + '/' + filename));

    networkRequest->setRawHeader("Content-Type", "text/calendar; charset=utf-8");
    networkRequest->setRawHeader("Content-Length", contentlength);

    QNetworkReply *networkReply = networkAccessManager->put(*networkRequest, buffer);

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

void ConnectionManager::makeCustomRequest(QString requestString, const QString &method) {

    if (networkRequest != nullptr) {
        return;
    }
    QBuffer *buffer = new QBuffer();

    buffer->open(QIODevice::ReadWrite);

    int buffersize = buffer->write(requestString.toUtf8());
    buffer->seek(0);
    buffer->size();

    QByteArray contentlength;
    contentlength.append(buffersize);

    QString authorization = "Basic ";
    authorization.append(
            (username + ":" + password).toUtf8().toBase64());

    if (!networkRequest) {
        networkRequest = new QNetworkRequest;
    }
    networkRequest->setUrl(serverUrl);
    networkRequest->setRawHeader("User-Agent", "CalendarClient_CalDAV");
    networkRequest->setRawHeader("Authorization", authorization.toUtf8());
    networkRequest->setRawHeader("Depth", "1");
    networkRequest->setRawHeader("Prefer", "return-minimal");
    networkRequest->setRawHeader("Content-Type", "text/xml; charset=utf-8");
    networkRequest->setRawHeader("Content-Length", contentlength);


    QSslConfiguration conf = networkRequest->sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    networkRequest->setSslConfiguration(conf);

    networkAccessManager->sendCustomRequest(*networkRequest, QByteArray(method.toUtf8()),
                                            buffer);
}

void ConnectionManager::checkctag(QNetworkReply *reply) {
    disconnect(connectionToGetCtag);
    if (reply != nullptr) {
        QByteArray answer = reply->readAll();
        QString answerString = QString::fromUtf8(answer);
       
        QNetworkReply::NetworkError error = reply->error();
        const QString &errorString = reply->errorString();
        if (error == QNetworkReply::NoError) {
            const int startPosition = answerString.indexOf("<cs:getctag>");
            const int endPosition = answerString.indexOf("</cs:getctag>");
            QString ctagString = answerString.mid(startPosition, endPosition - startPosition);
            const int startctag = ctagString.lastIndexOf('/');
            ctagString = ctagString.mid(startctag + 1, -1);

            if (ctag != ctagString.toInt()) { //something is changed
                ctag = ctagString.toInt();
                std::cout << "New ctag: " << ctag << '\n';
                // get ctag with etag request
                // connect to parser

                if (networkRequest != nullptr) {
                    return;
                }
                QBuffer *buffer = new QBuffer();

                buffer->open(QIODevice::ReadWrite);

                QString requestString =
                        "<c:calendar-query xmlns:d=\"DAV:\" xmlns:c=\"urn:ietf:params:xml:ns:caldav\">\n"
                        " <d:prop>\n"
                        "    <d:getetag />\n"
                        "  </d:prop>\n"
                        "<c:filter>\n"
                        "<c:comp-filter name=\"VCALENDAR\"/>\n"
                        "</c:filter>\n"
                        "</c:calendar-query>";

                int buffersize = buffer->write(requestString.toUtf8());
                buffer->seek(0);
                buffer->size();

                QByteArray contentlength;
                contentlength.append(buffersize);

                QString authorization = "Basic ";
                authorization.append(
                        (username + ":" + password).toUtf8().toBase64());

                if (!networkRequest) {
                    networkRequest = new QNetworkRequest;
                }
                networkRequest->setUrl(serverUrl);
                networkRequest->setRawHeader("User-Agent", "CalendarClient_CalDAV");
                networkRequest->setRawHeader("Authorization", authorization.toUtf8());
                networkRequest->setRawHeader("Depth", "1");
                networkRequest->setRawHeader("Prefer", "return-minimal");
                networkRequest->setRawHeader("Content-Type", "text/xml; charset=utf-8");
                networkRequest->setRawHeader("Content-Length", contentlength);


                QSslConfiguration conf = networkRequest->sslConfiguration();
                conf.setPeerVerifyMode(QSslSocket::VerifyNone);
                networkRequest->setSslConfiguration(conf);

                networkAccessManager->sendCustomRequest(*networkRequest, QByteArray("REPORT"), buffer);

                connectionToGetEtag = connect(networkAccessManager, &QNetworkAccessManager::finished, this,
                                              &ConnectionManager::getCalendarObjectUIDUpdated);
            }

        } else {
            std::cerr << "checkctag: " << errorString.toStdString() << '\n';
        }
    }

}

void ConnectionManager::checkChanges() {
    QString requestString =
            "<d:propfind xmlns:d=\"DAV:\" xmlns:cs=\"http://calendarserver.org/ns/\">\r\n"
            " <d:prop>\r\n"
            "    <d:displayname />\r\n"
            "    <cs:getctag />\r\n"
            "    <d:sync-token />\r\n "
            "  </d:prop>\r\n"
            "</d:propfind>";
    makeCustomRequest(requestString, "PROPFIND");
    connectionToGetCtag = connect(networkAccessManager, &QNetworkAccessManager::finished, this,
                                  &ConnectionManager::checkctag);

    //get ctag with ctag request
    // connect to ctag parser
}

void ConnectionManager::getCalendarObjectUIDUpdated(QNetworkReply *reply) {
    disconnect(connectionToGetEtag);
    if (reply != nullptr) {
        QByteArray answer = reply->readAll();
        QString answerString = QString::fromUtf8(answer);
        QNetworkReply::NetworkError error = reply->error();
        const QString &errorString = reply->errorString();

        std::cout << answerString.toStdString() << '\n';

        if (error == QNetworkReply::NoError) {
            std::cout << "get Calendar Object UID Updated: \n" << answerString.toStdString() << "\n\n";
        } else {
            std::cerr << errorString.toStdString() << '\n';
        }
    }
}