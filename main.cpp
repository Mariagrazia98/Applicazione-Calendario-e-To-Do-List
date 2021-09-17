#include <QApplication>
#include <QPushButton>
#include <QHBoxLayout>
#include <iostream>
#include "loginform.h"
#include "mainwindow.h"
#include "calendar.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    //MainWindow mainWindow;
    LoginForm loginForm;
    ConnectionManager*  connectionManager=new ConnectionManager();
    loginForm.setConnectionManager(connectionManager);
    if(loginForm.exec() == QDialog::Accepted)
    {
        Calendar calendar(nullptr,connectionManager);
        //mainWindow.show();
        //QHBoxLayout* mainlayout = new QHBoxLayout();
        //mainlayout->addWidget(calendar);
        //mainWindow.setLayout(mainlayout);
        calendar.show();
        //calendar.setPassword(loginForm.getPassword());
    }
    //Calendar calendar;
    //calendar.show();
    return QApplication::exec();
}
