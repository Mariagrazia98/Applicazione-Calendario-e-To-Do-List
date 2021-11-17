#ifndef TASKFORM_H
#define TASKFORM_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QBoxLayout>
#include <QGroupBox>
#include <QNetworkAccessManager>
#include <QBuffer>
#include <QMessageBox>
#include <QNetworkReply>
#include <QAuthenticator>

#include "../Model/calendarobject.h"
#include "../Model/calendarevent.h"
#include "../Controller/connectionManager.h"

namespace Ui {
    class TaskForm;
}

/**
 *  @brief Form to add/modify a VEVENT or a VTODO in the selected calendar
 */

class TaskForm : public QWidget {
Q_OBJECT

public:
    /* Constructor */

    /**
    *
    * @param connectionManagers a Map where the key is the name of the Calendar object and
     * the value is the corresponding ConnectionManager object
    * @param calendarObject if the TaskForm is used to add a task/event, this parameter is
     * null, if it is used to modify an existing task/event this parameter contains a pointer
     * to the CalendarObject we want to modify
    */

    TaskForm(QMap<QString, std::shared_ptr<ConnectionManager>> connectionManagers,
             CalendarObject *calendarObject = nullptr);

    /* Destructor */

    ~TaskForm();

    /**
    *  @brief initializes beginDateTime, expireDateTime and untilDate fields in the form
     *  with the value passed as a parameter
    */
    void setDate(const QDate &date);

private
    slots:

    /// @brief "Cancel" button clicked callback
    void on_buttonBox_rejected();

    /// @brief "Save" button clicked callback
    void on_buttonBox_accepted();

    /**
     * @brief callback called when the user selects if he wants to add
     * a task or an event
     */
    void on_comboBox_currentIndexChanged(int index);

    /**
     * @brief callback called when the user chooses a beginDateTime,
     * it updates the value of expireDateTime
     */
    void on_beginDateTime_dateTimeChanged(const QDateTime &dateTime);

    /**
     * @brief callback called when the user chooses a number of repetition,
     * if this number is greater than 0 the untilDate field is visible
     */
    void onNumRepetitionChanged(int i);

    /// @brief callback to handle the response of the insert/update request
    void handleUploadFinished(QNetworkReply *reply);

signals:

    /// @brief callback called when TaskForm is closed
    void taskFormClosed();

    /// @brief callback called when task/event is successfully added/modified
    void taskUploaded(const QString calendarName);

private:
    Ui::TaskForm *ui;

    QMap<QString, std::shared_ptr<ConnectionManager>> connectionManagers;
    // map of the connection managers related to all chosen Calendar objects

    QMetaObject::Connection connectionToFinish;
    // about the management of the response corresponding to the insert/update response

    std::shared_ptr<CalendarObject> calendarObject;
    // null if the user wants to add a new CalendarObject, otherwise the CalendarObject the user wants to modify

    QString calendarName;
    // name of the calendar to which the created/modified event belongs to

    void closeEvent(QCloseEvent *event) override;
};

#endif // TASKFORM_H
