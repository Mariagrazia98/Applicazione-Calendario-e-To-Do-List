#ifndef CALENDAROBJECTWIDGET_H
#define CALENDAROBJECTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QTextBrowser>
#include <QDateTime>

#include "../Model/calendarobject.h"
#include "../Model/calendarevent.h"
#include "../Model/calendartodo.h"
#include "taskform.h"
#include "eliminationtaskdialog.h"
#include "calendarwidget.h"

namespace Ui {
    class CalendarObjectWidget;
}
/**
 *  @brief Widget which belongs to a CalendarWidget and which displays details of
 *  a CalendarObject. It offers also the possibility to modify or delete the CalendarObject
 *  and if this is a CalendarToDo to check it as completed.
 */
class CalendarObjectWidget : public QWidget {
Q_OBJECT

public:
    /* Constructor */

    /**
    * @param parent the parent QWidget
    * @param calendarObject the calendarObject whose details are shown in this widget
    * @param connectionManagers a Map where the key is the name of the Calendar object and
    * the value is the corresponding ConnectionManager object
    */
    CalendarObjectWidget(QWidget *parent, std::shared_ptr<CalendarObject> calendarObject,
                         QMap<QString, std::shared_ptr<ConnectionManager>> connectionManagers);

    /* Destructor */

    ~CalendarObjectWidget();

    /// @brief Set up the user interface
    void setupUI();

private
    slots:

    /// @brief ModifyIcon clicked callback
    void onModifyButtonClicked();

    /// @brief DeleteIcon clicked callback
    void onRemoveButtonClicked();

    /// @brief Complete check callback
    void onCheckBoxToggled(bool checked);

    /**
     * @brief handles the response of the delete/update
     * (in case of checked as completed) request
     */
    void manageResponse(QNetworkReply *reply);

    /// @brief called when a CalendarObject is successfully modified
    void onTaskModified();

    /// @brief called after EliminationTaskDialog is shown and "OK" button is clicked
    void handleDeleteRecurrencies(int type);

signals:
    /// @brief emitted when the CalendarObject is successfully modified
    void taskModified(const QString calendarName);

    /// @bried emitted when the CalendarObject is successfully deleted
    void taskDeleted(CalendarObject &obj);

private:
    /// @brief called to display CalendarObject details
    void setupText();

    /// @brief called to display delete and modify icons
    void setupButtons();

    /// @bried called when calendarObject has no repetitions and the user decides to delete it
    void deleteCalendarObject();

    std::shared_ptr <CalendarObject> calendarObject;
    // the calendarObject whose details are shown in this widget

    QMetaObject::Connection connectionToFinish;
    // about the management of the response corresponding to the delete/update response

    QMetaObject::Connection connectionToObjectModified;
    // about the management of the response corresponding to the update response

    QMap<QString, std::shared_ptr<ConnectionManager>> connectionManagers;
    // map of the connection managers related to all chosen Calendar objects

    /* UI */

    Ui::CalendarObjectWidget *ui;
    QHBoxLayout *displayLayout;
    QVBoxLayout *buttonsLayout;
    QCheckBox *checkBox;
    QTextBrowser *textBrowser;
    QPushButton *modifyButton;
    QPushButton *removeButton;
};

#endif // CALENDAROBJECTWIDGET_H
