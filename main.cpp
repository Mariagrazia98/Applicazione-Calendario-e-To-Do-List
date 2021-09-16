#include <QApplication>
#include <QPushButton>
#include <QHBoxLayout>

#include "loginform.h"
#include "mainwindow.h"
#include "calendar.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    //MainWindow mainWindow;
    LoginForm loginForm;
    Calendar calendar;
    QObject::connect(&loginForm, &LoginForm::calendarObtained, &calendar, &Calendar::parseCalendar);
    if(loginForm.exec() == QDialog::Accepted)
    {
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