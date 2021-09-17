#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QAuthenticator>
#include "connectionManager.h"

namespace Ui {
class LoginForm;
}

class LoginForm : public QDialog
{
    Q_OBJECT

public:
    explicit LoginForm(QWidget *parent = nullptr);
    ~LoginForm();
    void setConnectionManager(ConnectionManager* connectionManager);
    void closeEvent(QCloseEvent* event);

private slots:
    void on_loginButton_clicked();
    void responseHandler(QNetworkReply *reply);
private:
    Ui::LoginForm *ui;
    bool loggedIn;
    ConnectionManager* connectionManager;


};

#endif // LOGINFORM_H
