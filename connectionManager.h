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

#include <iostream>
#include <QNetworkReply>

class ConnectionManager : public QObject {
Q_OBJECT
public:

    ConnectionManager(QString username = "", QString password = "", QString calendar = "default");

    void deleteCalendarObject(const QString &UID);

    void setUsername(QString username);

    void setPassword(QString password);

    void addOrUpdateCalendarObject(const QString &request, const QString &UID);

public slots:

    void authenticationRequired(QNetworkReply *, QAuthenticator *);

private slots:

    void responseHandler(QNetworkReply *);

signals:

    void finished(QNetworkReply *reply);

private:
    QNetworkAccessManager *networkAccessManager;
    QString username;
public:
    const QString &getUsername() const;

    const QString &getPassword() const;

private:
    QString password;
    QString calendar;
public:
    const QString &getCalendar() const;

    void setCalendar(const QString &calendar);

private:
    QUrl serverUrl;
public:
    const QUrl &getServerUrl() const;

private:

    void setup();

    void updateUrl();
};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CONNECTIONMANAGER_H
