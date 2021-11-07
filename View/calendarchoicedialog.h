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
#include "../Controller/connectionManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CalendarChoiceDialog; }
QT_END_NAMESPACE

class CalendarChoiceDialog : public QDialog {
Q_OBJECT

public:
    explicit CalendarChoiceDialog(QWidget *parent = nullptr, std::shared_ptr<ConnectionManager> connectionManager = nullptr);

    ~CalendarChoiceDialog() override;

    QList<Calendar *> getSelectedCalendars();

public slots:

    void setupUI(QList<Calendar *> calendarsList);

    void checkBoxToggled(int state);

private:
    Ui::CalendarChoiceDialog *ui;

    std::shared_ptr<ConnectionManager> connectionManager;

    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QList<QCheckBox *> checkBoxes;
    QPushButton *pushButton;

    QList<Calendar *> calendarsList;

    int calendarSelected;
};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDARCHOICEDIALOG_H
