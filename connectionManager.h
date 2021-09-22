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

class ConnectionManager : public QObject {
Q_OBJECT
public:

    ConnectionManager(QString username = "", QString password = "", QString calendar="default");

    void getCalendarRequest();

    void deleteCalendarObject(const QString &UID);

    void setUsername(QString username);

    void setPassword(QString password);

    void addOrUpdateCalendarObject(const QString &request, const QString &UID);

private slots:

    void responseHandler(QNetworkReply *);

    void authenticationRequired(QNetworkReply *, QAuthenticator *);

signals:

    void finished(QNetworkReply *reply);

private:
    QNetworkAccessManager *networkAccessManager;
    QString username;
    QString password;
    QString calendar;
public:
    const QString &getCalendar() const;

    void setCalendar(const QString &calendar);

private:
    QUrl serverUrl;

    void setup();

    void updateUrl();
};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CONNECTIONMANAGER_H
