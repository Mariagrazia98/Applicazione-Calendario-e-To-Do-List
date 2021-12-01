//
// Created by manue on 04/10/2021.
//

#ifndef APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDARCHOICEDIALOG_H
#define APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDARCHOICEDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>
#include <QButtonGroup>
#include <QFormLayout>
#include "../Controller/connectionmanager.h"

class CalendarChoiceDialog : public QDialog {
Q_OBJECT

public:
    /* Constructor */

    /**
     *
     * @param parent the parent QWidget
     * @param connectionManager the ConnectionManager used by this Widget
     */
    explicit CalendarChoiceDialog(QWidget *parent = nullptr, std::shared_ptr<ConnectionManager> connectionManager = nullptr);

    /* Getter */

    /**
     *
     * @return the list of selected Calendars
     */
    QList<Calendar *> getSelectedCalendars();

public slots:

    /**
     * setup the UI accordinglu to list
     * @param list the list of selectable Calendars
     */
    void setupUI(QList<Calendar *> list);

    /**
     * checkBox toggle callback
     * @param state the CheckBox state
     */
    void checkBoxToggled(int state);

private:

    std::shared_ptr<ConnectionManager> connectionManager; // shared pointer to the ConnectionManager

    /* UI */

    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QList<QCheckBox *> checkBoxes;
    QPushButton *pushButton;

    QList<Calendar *> calendarsList; // list of selectable Calendars

    int calendarSelected; // number of total Calendars Selected
};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDARCHOICEDIALOG_H
