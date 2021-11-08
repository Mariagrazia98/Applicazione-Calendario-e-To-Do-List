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

/***
 * @brief Form to fill in order to share the calendar with another user.
 * @details There is not any invitation process implemented. So the notification
 * is not needed and the server behaves as if sharees immediately accept invitations.
 * The sharee, a user to whom a calendar has been shared, will have the rights of read and write.
 */

class ShareCalendarForm : public QDialog {
Q_OBJECT
public:
    /**
     * Constructor
     * @param Qwidget parent of the dialog. The default location od the QDialog is centered on top of the parent.
     * @param connectionManagers
     */
    ShareCalendarForm(QWidget *parent, QMap<QString, std::shared_ptr<ConnectionManager>> connectionManagers);

private:
    QWidget *parent;  /**< parent widget of the qdialog */
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QGridLayout *layout;
    QLabel *calendarLabel;
    QComboBox *calendar; /**< name of the calendar that the user want to sharee */
    QLabel *emailLabel;
    QLineEdit *email; /**< email of the the user to whom a calendar has been shared */
    QLabel *nameLabel;
    QLineEdit *name; /**< name of the the user to whom a calendar has been shared */
    QLabel *commentLabel;
    QLineEdit *comment; /**< comment about the sharing */
    QDialogButtonBox *dialogButtonBox;  /**< buttons for closing the dialog and for sharing the calendar */
    QMap<QString, std::shared_ptr<ConnectionManager>> connectionManagers;

    void setupUI();

    void closeEvent(QCloseEvent *event) override;

signals:

    void closing();

private slots:
 /*
  * 
  */
    void onAcceptButtonClicked();
};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_SHARECALENDARFORM_H
