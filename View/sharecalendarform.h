//
// Created by manue on 26/10/2021.
//

#ifndef APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_SHARECALENDARFORM_H
#define APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_SHARECALENDARFORM_H

#include <QDialog>
#include <QWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QLabel>
#include "../Controller/connectionManager.h"

class ShareCalendarForm : public QDialog {
Q_OBJECT
public:
    ShareCalendarForm(QWidget *parent, QMap<QString, std::shared_ptr<ConnectionManager>> connectionManagers);

private:
    QWidget *parent;
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QGridLayout *layout;
    QLabel *calendarLabel;
    QComboBox *calendar;
    QLabel *emailLabel;
    QLineEdit *email;
    QLabel *nameLabel;
    QLineEdit *name;
    QLabel *commentLabel;
    QLineEdit *comment;
    QDialogButtonBox *dialogButtonBox;
    QMap<QString, std::shared_ptr<ConnectionManager>> connectionManagers;

    void setupUI();

    void closeEvent(QCloseEvent *event) override;

signals:

    void closing();

private slots:

    void onAcceptButtonClicked();
};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_SHARECALENDARFORM_H
