#include <QApplication>
#include <QPushButton>
#include <QHBoxLayout>

#include "loginform.h"
#include "mainwindow.h"
#include "calendar.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    /*
    MainWindow mainWindow;
    LoginForm loginForm;
    if(loginForm.exec() == QDialog::Accepted)
    {
        //mainWindow.show();
        //QHBoxLayout* mainlayout = new QHBoxLayout();
        Calendar* calendar = new Calendar();
        //mainlayout->addWidget(calendar);
        //mainWindow.setLayout(mainlayout);
        calendar->show();
    }
    */
    Calendar calendar;
    calendar.show();
    return QApplication::exec();
}
