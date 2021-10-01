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

#include <iostream>

class ConnectionManager : public QObject {
Q_OBJECT
public:

    ConnectionManager(QString username = "", QString password = "", QString calendar="default");

    void getCalendarRequest();

    void deleteCalendarObject(const QString &UID);

    void setUsername(QString username);

    void setPassword(QString password);

    void addOrUpdateCalendarObject(const QString &request, const QString &UID);

    void getctag();

    const QString &getCalendar() const;

    void setCalendar(const QString &calendar);

    void tryLogin();

private slots:

    void responseHandler(QNetworkReply *);

    void authenticationRequired(QNetworkReply *, QAuthenticator *);

    void checkctag(QNetworkReply *reply);

    void onLoginRequestFinished(QNetworkReply *reply);

signals:
    void onFinished(QNetworkReply *reply);

    void loggedin(QNetworkReply *reply);

private:
    QNetworkAccessManager *networkAccessManager;
    QString username;
    QString password;
    QString calendar;
    QUrl serverUrl;

    QMetaObject::Connection connectionToGetCtag;
    QMetaObject::Connection connectionToLogin;

    int ctag;

    void setup();

    void updateUrl();

    void getUpdatedTasks();

    void parseAndUpdatectag(const QString& answerString);

    void makectagRequest();

};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CONNECTIONMANAGER_H
