#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QDialog>

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

private:
    Ui::LoginForm *ui;
    bool loggedIn;
};

#endif // LOGINFORM_H
