//
// Created by mary_ on 17/09/2021.
//

#ifndef APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CONNECTIONMANAGER_H
#define APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CONNECTIONMANAGER_H


#include <QObject>
#include <QNetworkAccessManager>
#include <QAuthenticator>
class ConnectionManager: public QObject {
    Q_OBJECT
public:

    ConnectionManager(QString username="", QString password="");
    void getCalendarRequest();
    void setUsername(QString username);
    void setPassword(QString password);
signals:
    void finished(QNetworkReply* reply);
private:
    QNetworkAccessManager* networkAccessManager;
    QString  username;
    QString password;

    void setup();
private slots:
    void responseHandler(QNetworkReply*);
    void authenticationRequired(QNetworkReply *, QAuthenticator *);

};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CONNECTIONMANAGER_H
