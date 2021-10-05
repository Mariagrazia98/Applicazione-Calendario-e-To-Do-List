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
#include "connectionManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CalendarChoiceDialog; }
QT_END_NAMESPACE

class CalendarChoiceDialog : public QDialog {
Q_OBJECT

public:
    explicit CalendarChoiceDialog(QWidget *parent = nullptr, ConnectionManager *connectionManager = nullptr);

    ~CalendarChoiceDialog() override;

    void setConnectionManager(ConnectionManager *connectionManager);

public slots:

    void setupUI(QList<Calendar *> calendarsList);
    void onGroupButtonClicked(QAbstractButton* button);
private:
    Ui::CalendarChoiceDialog *ui;

    ConnectionManager *connectionManager;

    QVBoxLayout *buttonsLayout;
    QButtonGroup* groupButton;
    QGridLayout *layout;
};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CALENDARCHOICEDIALOG_H
