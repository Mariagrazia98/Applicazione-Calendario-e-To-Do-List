//
// Created by mary_ on 17/09/2021.
//

#include <iostream>

#include "connectionManager.h"
#include "calendarwidget.h"

ConnectionManager::ConnectionManager(QString username, QString password) :
        networkAccessManager(new QNetworkAccessManager),
        username(username),
        password(password),
        calendar(new Calendar) {
    updateUrl();
    setup();
}

void ConnectionManager::setup() {
    //connect(networkAccessManager, &QNetworkAccessManager::finished, this, &ConnectionManager::responseHandler);
    connect(networkAccessManager, &QNetworkAccessManager::authenticationRequired, this,
            &ConnectionManager::authenticationRequired);
}

void ConnectionManager::getCalendarRequest() {
    QNetworkRequest networkRequest;

    networkRequest.setUrl(QUrl(serverUrl.toString() + "?export"));
    getCalendarReply = networkAccessManager->get(networkRequest);
    if (getCalendarReply) {
        connect(getCalendarReply, &QNetworkReply::finished, this, &ConnectionManager::onGetCalendarRequestFinished);
    }
}

void ConnectionManager::onGetCalendarRequestFinished() {
    if (getCalendarReply) {
        std::cout << "[ConnectionManager] CalendarWidget Ready\n";
        emit(calendarReady(getCalendarReply));
    }
}


void ConnectionManager::authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator) {
    authenticator->setUser(username);
    authenticator->setPassword(password);
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
    networkRequest.setRawHeader("Content-Type", "text/Calendar; charset=utf-8");
    networkRequest.setRawHeader("Content-Length", 0);

    deleteResourceNetworkReply = networkAccessManager->deleteResource(networkRequest);
    connect(deleteResourceNetworkReply, &QNetworkReply::finished, this, &ConnectionManager::onObjectDeleted);

    if (!deleteResourceNetworkReply) {
        std::cerr << "[ConnectionManager] deleteCalendarObject went wrong" << std::endl;
    } else {
        QMessageBox::information(nullptr, "ToDo", "Task deleted successfully");
    }
}

void ConnectionManager::onObjectDeleted() {
    emit(objectDeleted(deleteResourceNetworkReply));
}

void ConnectionManager::addOrUpdateCalendarObject(const QString &requestString, const QString &UID) {
    QBuffer * buffer = new QBuffer();

    buffer->open(QIODevice::ReadWrite);
    int buffersize = buffer->write(requestString.toUtf8());
    buffer->seek(0);
    buffer->size();

    QByteArray contentlength;
    contentlength.append(buffersize);

    QNetworkRequest networkRequest;

    QString filename = UID + ".ics";
    networkRequest.setUrl(QUrl(serverUrl.toString() + '/' + filename));

    networkRequest.setRawHeader("Content-Type", "text/Calendar; charset=utf-8");
    networkRequest.setRawHeader("Content-Length", contentlength);

    addOrUpdateCalendarObjectNetworkReply = networkAccessManager->put(networkRequest, buffer);

    if (addOrUpdateCalendarObjectNetworkReply) {
        connect(addOrUpdateCalendarObjectNetworkReply, &QNetworkReply::finished, this,
                &ConnectionManager::onInsertOrUpdateCalendarObject);
        /*connect(qNetworkReply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(handleUploadHTTPError())); */

        //m_UploadRequestTimeoutTimer.start(m_RequestTimeoutMS);
    } else {
        //QDEBUG << m_DisplayName << ": " << "ERROR: Invalid reply pointer when requesting URL.";
        std::cerr << "Invalid reply pointer when requesting URL\n";
    }
}

void ConnectionManager::onInsertOrUpdateCalendarObject() {
    if (addOrUpdateCalendarObjectNetworkReply) {
        emit(insertOrUpdatedCalendarObject(addOrUpdateCalendarObjectNetworkReply));
    }
}

const QString &ConnectionManager::getCalendarName() const {
    return calendar->getName();
}

void ConnectionManager::setCalendarName(const QString &calendarName) {
    calendar->setName(calendarName);
    updateUrl();
}

void ConnectionManager::updateUrl() {
    serverUrl = QUrl(
            "http://localhost/progettopds/calendarserver.php/calendars/" + username + '/' + calendar->getName());
}

void ConnectionManager::getctag() {
    makectagRequest();
    connectionToGetCtag = connect(networkAccessManager, &QNetworkAccessManager::finished, this,
                                  &ConnectionManager::checkctag);
}

void ConnectionManager::checkctag(QNetworkReply *reply) {
    disconnect(networkAccessManager, &QNetworkAccessManager::finished, this, &ConnectionManager::checkctag);
    if (reply != nullptr) {
        QByteArray answer = reply->readAll();
        QString answerString = QString::fromUtf8(answer);
        QNetworkReply::NetworkError error = reply->error();
        const QString &errorString = reply->errorString();
        if (error == QNetworkReply::NoError) {
            parseAndUpdatectag(answerString);
        } else {
            /*TODO errore qui */
            std::cerr << "checkctag: " << errorString.toStdString() << '\n';
        }
    }
}

/*
void ConnectionManager::tryLogin() {
    makectagRequest();
    connectionToLogin = connect(networkAccessManager, &QNetworkAccessManager::finished, this,
                                &ConnectionManager::onLoginRequestFinished);
}
 */

void ConnectionManager::getUpdatedTasks() {

}

void ConnectionManager::parseAndUpdatectag(const QString &answerString) {
    const int startPosition = answerString.indexOf("<cs:getctag>");
    const int endPosition = answerString.indexOf("</cs:getctag>");
    QString ctagString = answerString.mid(startPosition, endPosition - startPosition);
    const int startctag = ctagString.lastIndexOf('/');
    ctagString = ctagString.mid(startctag + 1, -1);
    int new_ctag = ctagString.toInt();
    if (calendar->getCtag() != new_ctag && new_ctag > 0) { //something is changed
        calendar->setCtag(new_ctag);
        emit(ctagChanged());
        std::cout << "new ctag: " << new_ctag << '\n';
    }
}

void ConnectionManager::makectagRequest() {
    std::cout << "[ConnectionManager] makectagRequest\n";
    QBuffer * buffer = new QBuffer();

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
    //networkRequest.setRawHeader("Authorization", authorization.toUtf8());
    networkRequest.setRawHeader("Depth", "0");
    networkRequest.setRawHeader("Prefer", "return-minimal");
    networkRequest.setRawHeader("Content-Type", "text/xml; charset=utf-8");
    networkRequest.setRawHeader("Content-Length", contentlength);

    QSslConfiguration conf = networkRequest.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    networkRequest.setSslConfiguration(conf);

    QNetworkReply * networkReply = networkAccessManager->sendCustomRequest(networkRequest, QByteArray("PROPFIND"),
                                                                           buffer);
}

/*
void ConnectionManager::onLoginRequestFinished(QNetworkReply *reply) {
    disconnect(connectionToLogin);
    std::cout << "[ConnectionManager] OnLoginRequestFInished\n";
    QByteArray answer = reply->readAll();
    QString answerString = QString::fromUtf8(answer);
    QNetworkReply::NetworkError error = reply->error();
    const QString &errorString = reply->errorString();
    if (error == QNetworkReply::NoError) {
        parseAndUpdatectag(answerString);
        emit(loggedin(reply));
    } else {
        std::cerr << "onLoginRequestFinished: " << errorString.toStdString() << '\n';
    }
}
*/

void ConnectionManager::getCalendarList() {
    QBuffer * buffer = new QBuffer();

    buffer->open(QIODevice::ReadWrite);

    QString requestString =
            "<d:propfind xmlns:d=\"DAV:\" xmlns:cs=\"http://calendarserver.org/ns/\" xmlns:c=\"urn:ietf:params:xml:ns:caldav\">\r\n"
            "   <d:prop>\r\n"
            "       <d:resourcetype />\n"
            "       <d:displayname />\n"
            "       <cs:getctag />\r\n"
            //"       <c:supported-Calendar-component-set />"
            "   </d:prop>\r\n"
            "</d:propfind>";

    int buffersize = buffer->write(requestString.toUtf8());
    buffer->seek(0);
    buffer->size();

    QByteArray contentlength;
    contentlength.append(buffersize);

    QString authorization = "Basic ";
    authorization.append((username + ":" + password).toUtf8().toBase64());

    QNetworkRequest networkRequest;
    networkRequest.setUrl("http://localhost/progettopds/calendarserver.php/calendars/" + username + '/');
    networkRequest.setRawHeader("User-Agent", "CalendarClient_CalDAV");
    //networkRequest.setRawHeader("Authorization", authorization.toUtf8());
    networkRequest.setRawHeader("Depth", "1");
    networkRequest.setRawHeader("Prefer", "return-minimal");
    networkRequest.setRawHeader("Content-Type", "text/xml; charset=utf-8");
    networkRequest.setRawHeader("Content-Length", contentlength);

    QSslConfiguration conf = networkRequest.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    networkRequest.setSslConfiguration(conf);

    getCalendarsListReply = networkAccessManager->sendCustomRequest(networkRequest, QByteArray("PROPFIND"),
                                                                    buffer);
    connect(getCalendarsListReply, &QNetworkReply::finished, this, &ConnectionManager::printCalendarsList);
}

void ConnectionManager::printCalendarsList() {
    QByteArray answer = getCalendarsListReply->readAll();
    QString answerString = QString::fromUtf8(answer);
    QNetworkReply::NetworkError error = getCalendarsListReply->error();
    const QString &errorString = getCalendarsListReply->errorString();
    if (error == QNetworkReply::NoError) {
        //std::cout << answerString.toStdString() << "\n\n";
        const int startPosition = answerString.indexOf("<?xml version=\"1.0\"?>");
        answerString = answerString.mid(startPosition, -1);
        QDomDocument document;
        document.setContent(answerString);
        //std::cout << "document: " << document.toString().toStdString() << "\n\n";
        QDomNodeList response = document.elementsByTagName("d:response");
        //std::cout << response.size() << " nodes\n";
        QList < Calendar * > calendarsList;
        for (int i = 1; i < response.size(); i++) { // first element is not useful
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
                            Calendar *calendar = new Calendar(hrefString, name, ctagString.toInt());
                            calendarsList.append(calendar);
                        }
                    }
                }
            }

        }
        // finished parsing
        emit(calendars(calendarsList));
    } else {
        std::cerr << "printCalendarsList: " << errorString.toStdString() << '\n';
    }
    emit(loggedin(getCalendarsListReply));
}

void ConnectionManager::setCalendar(Calendar *calendar) {
    this->calendar = calendar;
    updateUrl();
}



