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
    ConnectionManager connectionManager;
    LoginForm loginForm(nullptr, &connectionManager);
    Calendar calendar(nullptr, &connectionManager);
    if (loginForm.exec() == QDialog::Accepted) {
        //mainWindow.show();
        //QHBoxLayout* mainlayout = new QHBoxLayout();
        //mainlayout->addWidget(calendar);
        //mainWindow.setLayout(mainlayout);
        calendar.getCalendarRequest();
        calendar.show();
        //calendar.setPassword(loginForm.getPassword());
    }
    //Calendar calendar;
    //calendar.show();
    return QApplication::exec();
}
