//
// Created by mary_ on 17/09/2021.
//

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

#include "calendar.h"

class ConnectionManager : public QObject {
Q_OBJECT
public:

    ConnectionManager(QString username = "", QString password = "", QString calendar = "default");

    void getCalendarRequest();

    void deleteCalendarObject(const QString &UID);

    void setUsername(QString username);

    void setPassword(QString password);

    void addOrUpdateCalendarObject(const QString &request, const QString &UID);

    void getctag();

    const QString &getCalendar() const;

    void setCalendar(const QString &calendar);

    void tryLogin();

    void getCalendarList();

private slots:

    void onGetCalendarRequestFinished();

    void authenticationRequired(QNetworkReply *, QAuthenticator *);

    void checkctag(QNetworkReply *reply);

    void onLoginRequestFinished(QNetworkReply *reply);

    void onObjectDeleted();

    void onInsertOrUpdateCalendarObject();

signals:

    void onFinished(QNetworkReply *reply);

    void calendarReady(QNetworkReply *reply);

    void objectDeleted(QNetworkReply* reply);

    void loggedin(QNetworkReply *reply);

    void ctagChanged();

    void insertOrUpdatedCalendarObject(QNetworkReply *reply);

    void calendars(QList<Calendar*> calendarsList);

private:
    QNetworkAccessManager *networkAccessManager;
    QString username;
    QString password;
    QString calendar;
    QUrl serverUrl;

    QMetaObject::Connection connectionToGetCtag;
    QMetaObject::Connection connectionToLogin;

    QNetworkReply *getCalendarReply;
    QNetworkReply *deleteResourceNetworkReply;
    QNetworkReply *addOrUpdateCalendarObjectNetworkReply;
    QNetworkReply *getCalendarsListReply;


    int ctag;

    void setup();

    void updateUrl();

    void getUpdatedTasks();

    void parseAndUpdatectag(const QString &answerString);

    void makectagRequest();


    void printCalendarsList();
};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CONNECTIONMANAGER_H
