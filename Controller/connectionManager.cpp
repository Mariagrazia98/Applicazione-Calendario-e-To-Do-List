//
// Created by mary_ on 17/09/2021.
//

#include <iostream>

#include "connectionManager.h"
#include "../View/calendarwidget.h"

ConnectionManager::ConnectionManager(QString username, QString password) :
        networkAccessManager(new QNetworkAccessManager),
        username(username),
        password(password),
        calendar(new Calendar) {
    updateUrl();
    setup();
}

ConnectionManager::~ConnectionManager() {
    /* Destructor */
    delete calendar;
    delete networkAccessManager;
}

void ConnectionManager::setup() {
    /* Connection between requests which require authentication and authenticator's parameters setters */
    connect(networkAccessManager, &QNetworkAccessManager::authenticationRequired, this,
            &ConnectionManager::authenticationRequired);
}

void ConnectionManager::authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator) {
    /* Set username and password of the logged user */
    authenticator->setUser(username);
    authenticator->setPassword(password);
}

/* Setters and Getters*/

void ConnectionManager::setUsername(QString username) {
    this->username = username;
    updateUrl();
}

void ConnectionManager::setPassword(QString password) {
    this->password = password;
}

const QString &ConnectionManager::getUsername() const {
    return username;
}

const QString &ConnectionManager::getPassword() const {
    return password;
}

void ConnectionManager::setCalendar(Calendar *calendar) {
    this->calendar = calendar;
    updateUrl();
}

void ConnectionManager::setCalendarName(const QString &calendarName) {
    calendar->setName(calendarName);
    updateUrl();
}

const QString &ConnectionManager::getCalendarName() const {
    return calendar->getName();
}

/* End Setters and Getters */

void ConnectionManager::updateUrl() {
    serverUrl = QUrl("http://localhost/progettopds/calendarserver.php/calendars/"
            + username + '/' + calendar->getName());
}

void ConnectionManager::getCalendarList() {
    QBuffer *buffer = new QBuffer();

    buffer->open(QIODevice::ReadWrite);

    /* Composition of the request */

    /* Request body */
    QString requestString =
            "<d:propfind xmlns:d=\"DAV:\" xmlns:cs=\"http://calendarserver.org/ns/\" "
            " xmlns:c=\"urn:ietf:params:xml:ns:caldav\">\r\n"
            "   <d:prop>\r\n"
            "       <d:resourcetype />\n"
            "       <d:displayname />\n"
            "       <cs:getctag />\r\n"
            "   </d:prop>\r\n"
            "</d:propfind>";

    int buffersize = buffer->write(requestString.toUtf8());
    buffer->seek(0);
    buffer->size();

    /* Request headers */
    QByteArray contentlength;
    contentlength.append(buffersize);

    QString authorization = "Basic ";
    authorization.append((username + ":" + password).toUtf8().toBase64());

    QNetworkRequest networkRequest;
    networkRequest.setUrl("http://localhost/progettopds/calendarserver.php/calendars/" + username + '/');
    networkRequest.setRawHeader("User-Agent", "CalendarClient_CalDAV");
    networkRequest.setRawHeader("Depth", "1");
    networkRequest.setRawHeader("Prefer", "return-minimal");
    networkRequest.setRawHeader("Content-Type", "text/xml; charset=utf-8");
    networkRequest.setRawHeader("Content-Length", contentlength);

    QSslConfiguration conf = networkRequest.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    networkRequest.setSslConfiguration(conf);

    /* Composition of request end */

    /* Request send and connection to the handler of the response */
    getCalendarsListReply = networkAccessManager->sendCustomRequest(networkRequest, QByteArray("PROPFIND"), buffer);
    connect(getCalendarsListReply, &QNetworkReply::finished, this, &ConnectionManager::printCalendarsList);
}

void ConnectionManager::printCalendarsList() {
    QByteArray answer = getCalendarsListReply->readAll();
    QString answerString = QString::fromUtf8(answer);
    QNetworkReply::NetworkError error = getCalendarsListReply->error();
    const QString &errorString = getCalendarsListReply->errorString();
    if (error == QNetworkReply::NoError) {
        /* Success */
        const int startPosition = answerString.indexOf("<?xml version=\"1.0\"?>");
        answerString = answerString.mid(startPosition, -1);
        QDomDocument document;
        document.setContent(answerString);
        QDomNodeList response = document.elementsByTagName("d:response");
        calendarsList.clear();
        /* Parsing the response */
        for (int i = 1; i < response.size(); i++) {
            /* First element is not useful */
            QDomNode node = response.item(i);
            QDomElement href = node.firstChildElement("d:href");
            const QString hrefString = href.text();
            if (!href.isNull()) {
                const int startPosition = hrefString.indexOf(username + '/');
                const int endPosition = hrefString.lastIndexOf('/');
                QString name = hrefString.mid(startPosition + username.length() + 1,
                                              endPosition - (startPosition + username.length() + 1));
                QDomNode propstat = node.firstChildElement("d:propstat");
                if (!propstat.isNull()) {
                    QDomNode prop = propstat.firstChildElement("d:prop");
                    if (!prop.isNull()) {
                        QDomElement ctag = prop.firstChildElement("cs:getctag");
                        if (!ctag.isNull()) {
                            const int startPosition = ctag.text().lastIndexOf("sync/");
                            QString ctagString = ctag.text().mid(startPosition + 5, -1);
                            /* Creation of a new Calendar object */
                            Calendar *calendar = new Calendar(hrefString, name, ctagString.toInt());
                            calendarsList.append(calendar);
                        }
                    }
                }
            }
        }
        /* OnCalendarReady parsing */
        emit(calendars(calendarsList));
    } else {
        /* Error */
        std::cerr << "printCalendarsList: " << errorString.toStdString() << '\n';
        std::cerr << answerString.toStdString() << '\n';
    }
    emit(loggedin(getCalendarsListReply));
}

void ConnectionManager::getCalendarRequest() {
    /* GET request for the calendar characterized by serverUrl */
    QNetworkRequest networkRequest;
    networkRequest.setUrl(QUrl(serverUrl.toString() + "?export"));
    getCalendarReply = networkAccessManager->get(networkRequest);
    if (getCalendarReply) {
        connect(getCalendarReply, &QNetworkReply::finished, this, &ConnectionManager::onGetCalendarRequestFinished);
    }
}

void ConnectionManager::onGetCalendarRequestFinished() {
    emit(calendarReady(getCalendarReply));
}

void ConnectionManager::deleteCalendarObject(const QString &UID) {
    if (UID.isEmpty()) {
        return;
    }
    /* Composing request, body is empty */
    QNetworkRequest networkRequest;
    networkRequest.setUrl(QUrl(serverUrl.toString() + "/" + UID + ".ics"));
    networkRequest.setRawHeader("Content-Type", "text/Calendar; charset=utf-8");
    networkRequest.setRawHeader("Content-Length", 0);

    /* DELETE request send and connection to the handler of the response */
    deleteResourceNetworkReply = networkAccessManager->deleteResource(networkRequest);
    connect(deleteResourceNetworkReply, &QNetworkReply::finished, this, &ConnectionManager::onObjectDeleted);

    if (deleteResourceNetworkReply != nullptr) {
        /* Success */
        QMessageBox::information(nullptr, "Task Deleted", "Task deleted successfully");
    } else {
        /* Error */
        QMessageBox::warning(nullptr, "Task Deleted", "Could not delete selected object");
    }
}

void ConnectionManager::onObjectDeleted() {
    emit(objectDeleted(deleteResourceNetworkReply));
}

void ConnectionManager::addOrUpdateCalendarObject(const QString &requestString, const QString &UID) {
    /* Composing request, in this case the response body is already done and passed as a parameter */
    QBuffer *buffer = new QBuffer();

    buffer->open(QIODevice::ReadWrite);
    int buffersize = buffer->write(requestString.toUtf8());
    buffer->seek(0);
    buffer->size();

    /* Request headers */
    QByteArray contentlength;
    contentlength.append(buffersize);

    QNetworkRequest networkRequest;
    QString filename = UID + ".ics";
    networkRequest.setUrl(QUrl(serverUrl.toString() + '/' + filename));
    networkRequest.setRawHeader("Content-Type", "text/Calendar; charset=utf-8");
    networkRequest.setRawHeader("Content-Length", contentlength);

    /* PUT request send and connection to the handler of the response */
    addOrUpdateCalendarObjectNetworkReply = networkAccessManager->put(networkRequest, buffer);
    if (addOrUpdateCalendarObjectNetworkReply) {
        connect(addOrUpdateCalendarObjectNetworkReply, &QNetworkReply::finished, this,
                &ConnectionManager::onInsertOrUpdateCalendarObject);
    } else {
        /* Error */
        std::cerr << "Invalid reply pointer when requesting URL\n";
    }
}

void ConnectionManager::onInsertOrUpdateCalendarObject() {
    if (addOrUpdateCalendarObjectNetworkReply) {
        emit(insertOrUpdatedCalendarObject(addOrUpdateCalendarObjectNetworkReply));
    }
}

void ConnectionManager::getctag() {
    makectagRequest();
    connectionToGetCtag = connect(networkAccessManager, &QNetworkAccessManager::finished, this,
                                  &ConnectionManager::checkctag);
}

void ConnectionManager::makectagRequest() {
    /* Composing request */

    /* Body request */
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

    /* Request headers */
    QByteArray contentlength;
    contentlength.append(buffersize);

    QString authorization = "Basic ";
    authorization.append((username + ":" + password).toUtf8().toBase64());

    QNetworkRequest networkRequest;
    networkRequest.setUrl(serverUrl);
    networkRequest.setRawHeader("User-Agent", "CalendarClient_CalDAV");
    networkRequest.setRawHeader("Depth", "0");
    networkRequest.setRawHeader("Prefer", "return-minimal");
    networkRequest.setRawHeader("Content-Type", "text/xml; charset=utf-8");
    networkRequest.setRawHeader("Content-Length", contentlength);

    QSslConfiguration conf = networkRequest.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    networkRequest.setSslConfiguration(conf);

    /* GET custom request send, connection to the handler of the response is set by getctag() */
    networkAccessManager->sendCustomRequest(networkRequest, QByteArray("PROPFIND"), buffer);
}

void ConnectionManager::checkctag(QNetworkReply *reply) {
    disconnect(networkAccessManager, &QNetworkAccessManager::finished, this, &ConnectionManager::checkctag);
    if (reply != nullptr) {
        QByteArray answer = reply->readAll();
        QString answerString = QString::fromUtf8(answer);
        QNetworkReply::NetworkError error = reply->error();
        const QString &errorString = reply->errorString();
        if (error == QNetworkReply::NoError) {
            /* Success */
            parseAndUpdatectag(answerString);
        } else {
            /* Error */
            std::cerr << "checkctag: " << errorString.toStdString() << '\n';
        }
    }
}

void ConnectionManager::parseAndUpdatectag(const QString &answerString) {
    const int startPosition = answerString.indexOf("<cs:getctag>");
    const int endPosition = answerString.indexOf("</cs:getctag>");
    QString ctagString = answerString.mid(startPosition, endPosition - startPosition);
    const int startctag = ctagString.lastIndexOf('/');
    ctagString = ctagString.mid(startctag + 1, -1);
    int new_ctag = ctagString.toInt();
    if (calendar->getCtag() != new_ctag && new_ctag > 0) {
        /* The new ctag of the calendar is different from the last saved one */
        calendar->setCtag(new_ctag);
        emit(ctagChanged(calendar->getName()));
        std::cout << "new ctag: " << new_ctag << '\n';
    }
}

void ConnectionManager::makeShareCalendarRequest(const QString &calendar, const QString &email,
                                                 const QString &displayName) {
    /* Composing request */

    /* Body request */
    QBuffer *buffer = new QBuffer();

    buffer->open(QIODevice::ReadWrite);

    QString requestString =
            "<D:share-resource xmlns:D=\"DAV:\">\n"
            "     <D:sharee>\n"
            "       <D:href>" + email + "</D:href>\n"
            "       <D:prop>\n"
            "         <D:displayname>" + displayName + "</D:displayname>\n"
            "       </D:prop>\n"
            "       <D:share-access>\n"
            "         <D:read-write />\n"
            "       </D:share-access>\n"
            "     </D:sharee>\n"
            "   </D:share-resource>";

    int buffersize = buffer->write(requestString.toUtf8());
    buffer->seek(0);
    buffer->size();

    /* Headers request */

    QByteArray contentlength;
    contentlength.append(buffersize);

    QString authorization = "Basic ";
    authorization.append((username + ":" + password).toUtf8().toBase64());

    QNetworkRequest networkRequest;
    networkRequest.setUrl("http://localhost/progettopds/calendarserver.php/calendars/" + username + '/' + calendar);
    networkRequest.setRawHeader("User-Agent", "CalendarClient_CalDAV");
    networkRequest.setRawHeader("Content-Type", "application/davsharing+xml; charset=utf-8");
    networkRequest.setRawHeader("Content-Length", contentlength);

    QSslConfiguration conf = networkRequest.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    networkRequest.setSslConfiguration(conf);

    /* POST custom request send and connection to the handler of the response*/
    shareCalendarRequestReply = networkAccessManager->sendCustomRequest(networkRequest, QByteArray("POST"), buffer);
    connect(shareCalendarRequestReply, &QNetworkReply::finished, this, &ConnectionManager::shareCalendarDone);
}

void ConnectionManager::shareCalendarDone() {
    QNetworkReply::NetworkError error = shareCalendarRequestReply->error();
    const QString &errorString = shareCalendarRequestReply->errorString();
    if (error == QNetworkReply::NoError) {
        /* Success */
        QByteArray answer = shareCalendarRequestReply->readAll();
        QString answerString = QString::fromUtf8(answer);
        std::cout << "answer: " << answer.toStdString() << '\n';
    } else {
        /* Error */
        std::cerr << "shareCalendarDone: " << errorString.toStdString() << '\n';
    }
}