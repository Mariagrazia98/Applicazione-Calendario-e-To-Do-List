/**
 * @file connectionManager
 * @author Mariagrazia Paladino, Manuel Pepe, Adriana Provenzano
 * @date 17 Sep 2021
 * @brief handles connections between server and widgets
 *
 *
 */

#ifndef APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CONNECTIONMANAGER_H
#define APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CONNECTIONMANAGER_H


#include <QObject>
#include <QNetworkAccessManager>
#include <QAuthenticator>
#include <QMessageBox>
#include <QWidget>
#include <QBuffer>
#include <QMessageBox>
#include <QNetworkReply>
#include <QDomDocument>

#include <iostream>

#include "../Model/calendar.h"

class ConnectionManager : public QObject {
Q_OBJECT
public:

    ConnectionManager(QString username = "", QString password = "");

    void getCalendarRequest();

    /**
     * @brief deletes calendar object by UID
     *
     * @param UID the UID of the object to be deleted
     */
    void deleteCalendarObject(const QString &UID);

    void setUsername(QString username);

    void setPassword(QString password);

    const QString &getUsername() const;

    const QString &getPassword() const;

    /**
     * @brief add or update a Calendar Object
     *
     * executes a PUT request on the server built by TaskForm
     *
     * @param request the request built by TaskForm
     * @param UID the UID of the objects to be added
     */
    void addOrUpdateCalendarObject(const QString &request, const QString &UID);

    void getctag();

    const QString &getCalendarName() const;

    void setCalendarName(const QString &calendar);

    //void tryLogin();

    void getCalendarList();

    void setCalendar(Calendar *calendar);

    void makeShareCalendarRequest(const QString &email, const QString &displayName, const QString &comment);

private slots:

    void onGetCalendarRequestFinished();

    /**
     * @brief authenticates with username and password
     *
     * @param networkReply server reply
     * @param authenticator QAuthenticator object
     */
    void authenticationRequired(QNetworkReply *networkReply, QAuthenticator *authenticator);

    void checkctag(QNetworkReply *reply);

    /**
     * @brief tryLogin callback
     *
     *
     * check errors and emits the loggedin signal
     *
     * @param reply network reply to getctag request
     */
    //void onLoginRequestFinished(QNetworkReply *reply);

    void onObjectDeleted();

    void onInsertOrUpdateCalendarObject();

    void shareCalendarDone();

signals:

    void onFinished(QNetworkReply *reply);

    void calendarReady(QNetworkReply *reply);

    void objectDeleted(QNetworkReply *reply);

    void loggedin(QNetworkReply *reply);

    void ctagChanged(const QString);

    void insertOrUpdatedCalendarObject(QNetworkReply *reply);

    void calendars(QList<Calendar *> calendarsList);

private:
    QNetworkAccessManager *networkAccessManager;
    QString username;
    QString password;
    QUrl serverUrl;
    QList<Calendar*> calendarsList;

    QMetaObject::Connection connectionToGetCtag;
    QMetaObject::Connection connectionToLogin;

    QNetworkReply *getCalendarReply;
    QNetworkReply *deleteResourceNetworkReply;
    QNetworkReply *addOrUpdateCalendarObjectNetworkReply;
    QNetworkReply *getCalendarsListReply;
    QNetworkReply *shareCalendarRequestReply;


    Calendar *calendar;

    void setup();

    void updateUrl();

    void getUpdatedTasks();

    void parseAndUpdatectag(const QString &answerString);

    void makectagRequest();


    void printCalendarsList();

};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CONNECTIONMANAGER_H
