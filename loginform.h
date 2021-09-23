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
#include "connectionManager.h"

namespace Ui {
    class LoginForm;
}

class LoginForm : public QDialog {
Q_OBJECT

public:
    explicit LoginForm(QWidget *parent = nullptr, ConnectionManager *connectionManager = nullptr);

    ~LoginForm();

    void setConnectionManager(ConnectionManager *connectionManager);

private slots:

    void onLoginButtonClicked();

    void responseHandler(QNetworkReply *reply);

private:
    Ui::LoginForm *ui;
    ConnectionManager *connectionManager;

    QGroupBox *groupBox;
    QGridLayout *layout;
    QFormLayout *formLayout;

    QLabel *userLabel;
    QLineEdit *user;
    QLabel *passwordLabel;
    QLineEdit *password;
    QLabel *calendarLabel;
    QLineEdit *calendar;
    QDialogButtonBox* dialogButtonBox;

    QMetaObject::Connection connection;
};

#endif // LOGINFORM_H
