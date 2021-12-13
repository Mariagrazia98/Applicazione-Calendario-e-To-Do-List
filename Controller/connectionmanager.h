/**
 * @file connectionManager
 * @author Mariagrazia Paladino, Manuel Pepe, Adriana Provenzano
 * @date 17 Sep 2021
 * @brief handles connections between server and widgets
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

/**
 *  @brief manager of server requests and response
 */
class ConnectionManager : public QObject {
    Q_OBJECT
public:

    /**
     * Constructor
     * @param username of the logged user
     * @param password of the logged user
     */
    ConnectionManager(QString username = "", QString password = "");

    /**
     * Destructor
     */
    virtual ~ConnectionManager();

    /**
     * @brief get the calendar object at serverUrl
     * @details it is based on the calendar set for the current connectionManager
     */
    void getCalendarRequest();

    /**
     * @brief deletes calendar object by UID
     * @param UID the UID of the object to be deleted
     */
    void deleteCalendarObject(const QString &UID);

    /**
     * Setter
     * @param username of the logged user
     */
    void setUsername(const QString &username);

    /**
     * Setter
     * @param password of the logged user
     */
    void setPassword(const QString &password);

    /**
     * Getter
     * @return username of the logged user (QString)
     */
    const QString &getUsername() const;

    /**
     * Getter
     * @return password of the logged user (QString)
     */
    const QString &getPassword() const;

    /**
     * @brief add or update a Calendar Object
     * @details executes a PUT request on the server built by CalendarObjectForm
     * @param request the request built by CalendarObjectForm
     * @param UID the UID of the objects to be added
     */
    void addOrUpdateCalendarObject(const QString &request, const QString &UID);

    /**
     * @brief calls makectagRequest() and connects to checkctag() when the response is ready
     * @details it is based on the name of the calendar set for the current connectionManager
     */
    void getctag();

    /**
     * Getter
     * @return the name of the calendar which characterizes the current connectionManager
     */
    const QString &getCalendarName() const;

    /**
     * Getter
     * @return the dispayed name of the calendar which characterizes the current connectionManager
     */
    const QString &getCalendarDisplayName() const;

    /**
     * Setter
     * @param calendar the name of the calendar which will characterize the current connectionManager
     */
    void setCalendarName(const QString &calendar);

    /**
     * @brief get the list of calendar objects which the logged user has
     */
    void getCalendarList();

    /**
    * Setter
    * @param calendar the calendar object which will characterize the current connectionManager
    */
    void setCalendar(Calendar *calendar);

    /**
     * @param calendarString the name of the calendar which the user wants to share with someone
     * @param email email address of the user with who the logged user wants to share his calendar
     * @param displayName a human- readable string identifying the user.
     */
    void makeShareCalendarRequest(const QString &calendarString, const QString &email, const QString &displayName);


private
    slots:

            /**
             * @details when called it emits a signal which says that the calendar is ready and passes
             * the response to a method which will parse the GET Calendar response
             */
            void onGetCalendarRequestFinished();

    /**
     * @brief it authenticates with username and password
     * @param networkReply server reply
     * @param authenticator QAuthenticator object
     */
    void authenticationRequired(QNetworkReply *networkReply, QAuthenticator *authenticator);

    /**
     * @brief it handles the response of the ctag request and calls parseAndUpdatectag method
     * @param reply server reply
     */
    void checkctag(QNetworkReply *reply);

    /**
     * @brief it emits a signal which says that a calendar object has been deleted
     */
    void onObjectDeleted();

    /**
     * @brief it emits a signal which says that a calendar object has been added or updated
     */
    void onInsertOrUpdateCalendarObject();

    /**
     * @brief it handles the response of a share calendar request
     */
    void shareCalendarDone();

    signals:

            /**
             * @param reply server reply
             */
            void onFinished(QNetworkReply * reply);

    /**
     * @brief emitted when the response to GET calendar request is ready
     * @param reply server reply
     */
    void calendarReady(QNetworkReply *reply);

    /**
     * @brief emitted when the response to delete calendar object request is ready
     * @param reply server reply
     */
    void objectDeleted(QNetworkReply *reply);

    /**
     * @brief emitted when the response to insert or update a calendar object request is ready
     * @param reply server reply
     */
    void insertOrUpdatedCalendarObject(QNetworkReply *reply);

    /**
     * @brief emitted when the response to GET calendars list request is ready
     * @param reply server reply
     */
    void loggedin(QNetworkReply *reply);

    /**
     * @brief emitted when the new ctag is different from the last saved one
     */
    void ctagChanged(const QString calendarName);

    /**
     * @brief emitted when the list of calendars is ready
     * @param calendarsList list of Calendar objects
     */
    void calendars(QList<Calendar *> calendarsList);

private:
    /** it allows the application to send network requests and receive replies. */
    QNetworkAccessManager *networkAccessManager;

    /** url of the current connectionManager calendar */
    QUrl serverUrl;

    /** calendars of the logged user */
    QList<Calendar *> calendarsList;

    /** username of the logged user */
    QString username;
    /** password of the logged user */
    QString password;

    /** reply to GET calendar request */
    QNetworkReply *getCalendarReply;
    /** reply to delete calendar object request */
    QNetworkReply *deleteResourceNetworkReply;
    /** reply to add/update calendar object request */
    QNetworkReply *addOrUpdateCalendarObjectNetworkReply;
    /** reply to GET calendars list request */
    QNetworkReply *getCalendarsListReply;
    /** reply to share calendar request */
    QNetworkReply *shareCalendarRequestReply;

    /** calendar object which characterizes the current connectionManager */
    Calendar *calendar;


private:

    /**
     * @details it sets up the connection beewteen requests which require authentication
     * and the authenticator's setter methods
     */
    void setup();

    /**
     * @details it updates the serverUrl according to the current logged user and the calendar name
     */
    void updateUrl();

    /**
     * @details it parses the GET ctag response and emits a signal if it has changed
     * @param answerString passed by checkctag, it is the server reply to GET ctag request
     */
    void parseAndUpdatectag(const QString &answerString);

    /**
     * @brief it sends a server request for having the current ctag
     */
    void makectagRequest();

    /**
     * @brief it parses the GET calendars list response
     */
    void parseCalendarsList();

};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CONNECTIONMANAGER_H
