#include "loginform.h"
#include "ui_loginform.h"

#include <QMessageBox>

LoginForm::LoginForm(QWidget *parent) :
    loggedIn(false),
    QDialog(parent),
    ui(new Ui::LoginForm)
{
    ui->setupUi(this);
}

LoginForm::~LoginForm()
{
    delete ui;
}

void LoginForm::closeEvent(QCloseEvent *event)
{
    if(!loggedIn){
        QApplication::quit();
    }
}

void LoginForm::on_loginButton_clicked()
{
    QString username = ui->usernameText->text();
    QString password = ui->passwordText->text();

    //TODO login
    if(username ==  "test" && password == "test") {
            QMessageBox::information(this, "Login", "Username and password is correct");
            //hide();
            loggedIn = true;
            accept();
            this->close();
        }
        else {
            QMessageBox::warning(this,"Login", "Username and password is not correct");
    }
}
