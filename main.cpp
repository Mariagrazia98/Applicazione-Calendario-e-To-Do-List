#include <QApplication>
#include <QHBoxLayout>
#include <QFile>

#include <iostream>
#include "loginform.h"
#include "calendar.h"


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    //Set the app style sheet
    /*
    QFile styleSheetStyle(":/QTheme/Fibers.qss");
    if (styleSheetStyle.exists()) {
        bool flag = styleSheetStyle.open(QFile::ReadOnly | QFile::Text);
        if (flag) {
            QString styleSheet = QLatin1String(styleSheetStyle.readAll());
            a.setStyleSheet(styleSheet);
        }
    } else {
        std::cerr << "Could not open stylesheet file\n";
    }*/

    ConnectionManager connectionManager;
    LoginForm loginForm(nullptr, &connectionManager);
    Calendar calendar(nullptr, &connectionManager);
    if (loginForm.exec() == QDialog::Accepted) {

        calendar.setupConnection();
        calendar.show();
    }
    return QApplication::exec();
}
