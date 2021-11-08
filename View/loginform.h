#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <memory>
#include "../Controller/connectionManager.h"
#include "calendarchoicedialog.h"

/**
 *  @brief Form to log in the iCalendar server
 *
 */

class LoginForm : public QDialog {
Q_OBJECT

public:
    /* Constructor */

    /**
     *
     * @param parent the parent QWidget
     * @param connectionManager the ConnectionManager used by this Widget
     */
    explicit LoginForm(QWidget *parent = nullptr,
                       std::shared_ptr<ConnectionManager> connectionManager = std::make_shared<ConnectionManager>());


private
    slots:

    /// @brief buttonClicked callback
    void onLoginButtonClicked();

    /**
     * @brief checks the login reply
     * @details if the reply has no errors, accept and close the widget
     */
    void responseHandler(QNetworkReply *reply);

private:
    std::shared_ptr<ConnectionManager> connectionManager; // the ConnectionManager used by this Widget

    /* UI */

    QGroupBox *groupBox;
    QGridLayout *layout;
    QFormLayout *formLayout;

    QLabel *userLabel;
    QLineEdit *user;
    QLabel *passwordLabel;
    QLineEdit *password;
    QDialogButtonBox *dialogButtonBox;
};

#endif // LOGINFORM_H
