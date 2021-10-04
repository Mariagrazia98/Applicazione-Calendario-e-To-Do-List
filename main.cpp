#include <QApplication>
#include <QHBoxLayout>
#include <QFile>

#include <iostream>
#include "loginform.h"
#include "calendarwidget.h"


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    //Set the app style sheet
    /*
    QFile styleSheetStyle(":/QTheme/Toolery.qss");
    if (styleSheetStyle.exists()) {
        bool flag = styleSheetStyle.open(QFile::ReadOnly | QFile::Text);
        if (flag) {
            QString styleSheet = QLatin1String(styleSheetStyle.readAll());
            a.setStyleSheet(styleSheet);
        }
    } else {
        std::cerr << "Could not open stylesheet file\n";
    }*/

    a.setWindowIcon(QIcon(":/resources/list.png"));

    ConnectionManager connectionManager;
    LoginForm loginForm(nullptr, &connectionManager);
    CalendarWidget calendar(nullptr, &connectionManager);
    if (loginForm.exec() == QDialog::Accepted) {

        calendar.setupConnection();
        calendar.setupTimer();
        calendar.show();
    }
    return QApplication::exec();
}
