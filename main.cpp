#include <QApplication>
#include <QHBoxLayout>
#include <QFile>
#include <QWidget>

#include <iostream>
#include "View/loginform.h"
#include "View/calendarwidget.h"
#include "Controller/connectionManager.h"
#include "View/calendarchoicedialog.h"


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

    ConnectionManager *connectionManager = new ConnectionManager();
    LoginForm loginForm(nullptr, connectionManager);
    CalendarWidget calendarWidget(nullptr);
    CalendarChoiceDialog calendarChoiceDialog(nullptr, connectionManager);
    Calendar::connect(connectionManager, &ConnectionManager::calendars, &calendarChoiceDialog,
                      &CalendarChoiceDialog::setupUI);
    if (loginForm.exec() == QDialog::Accepted) {
        calendarChoiceDialog.show();
        if (calendarChoiceDialog.exec() == QDialog::Accepted) {
            QList<Calendar *> calendars = calendarChoiceDialog.getSelectedCalendars();
            for (int i = 0; i < calendars.length(); ++i) {
                ConnectionManager *connectionManager_ = new ConnectionManager(connectionManager->getUsername(),
                                                                              connectionManager->getPassword());
                connectionManager_->setCalendar(calendars[i]);
                calendarWidget.addConnectionManager(connectionManager_);
            }
            calendarWidget.setupConnection();
            calendarWidget.setupTimer();
            calendarWidget.show();
        }
    }
    return QApplication::exec();
}
