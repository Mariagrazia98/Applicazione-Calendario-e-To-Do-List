#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QAuthenticator>

namespace Ui {
class LoginForm;
}

class LoginForm : public QDialog
{
    Q_OBJECT

public:
    explicit LoginForm(QWidget *parent = nullptr);
    ~LoginForm();

    void closeEvent(QCloseEvent* event);

private slots:
    void on_loginButton_clicked();

    void responseHandler(QNetworkReply*);
    void authenticationRequired(QNetworkReply *, QAuthenticator *);

    signals:
    void calendarObtained(QString calendar);


private:
    Ui::LoginForm *ui;
    bool loggedIn;

    /* Network functions */
    void setupRequest();
    void makeRequest();

    QNetworkAccessManager* networkAccessManager;
};

#endif // LOGINFORM_H
