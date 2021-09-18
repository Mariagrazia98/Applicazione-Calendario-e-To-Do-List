#include <QApplication>
#include <QPushButton>
#include <QHBoxLayout>
#include <iostream>
#include "loginform.h"
#include "mainwindow.h"
#include "calendar.h"
#include <QFile>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    //Set the app style sheet
    QFile styleSheetStyle(":/QTheme/Fibers.qss");
    if(styleSheetStyle.exists()){
        std::cout<<"esiste"<<std::endl;
    }
    bool flag= styleSheetStyle.open(QFile::ReadOnly | QFile::Text);
    if(flag){
        std::cout<<"file aperto per la lettura"<<std::endl;
    }
    QString styleSheet=QLatin1String(styleSheetStyle.readAll());
    a.setStyleSheet(styleSheet);


    //MainWindow mainWindow;
    ConnectionManager connectionManager;
    LoginForm loginForm(nullptr, &connectionManager);
    Calendar calendar(nullptr, &connectionManager);
    if (loginForm.exec() == QDialog::Accepted) {

        calendar.setupConnection();
        calendar.show();
    }
    return QApplication::exec();
}
