#include <QApplication>
#include <QHBoxLayout>
#include <QFile>
#include <QWidget>

#include <iostream>
#include <memory>
#include "View/loginform.h"
#include "View/calendarwidget.h"
#include "Controller/connectionmanager.h"
#include "View/calendarchoicedialog.h"


int main(int argc, char *argv[]) {
    QApplication application(argc, argv);

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

    // set the Application icon
    QApplication::setWindowIcon(QIcon(":/resources/list.png"));

    std::shared_ptr<ConnectionManager> connectionManager = std::make_shared<ConnectionManager>();
    LoginForm loginForm(nullptr, connectionManager);
    loginForm.resize(300,200);
    CalendarChoiceDialog calendarChoiceDialog(nullptr, connectionManager);
    Calendar::connect(connectionManager.get(), &ConnectionManager::calendars, &calendarChoiceDialog,
                      &CalendarChoiceDialog::setupUI);
    // when login is successful
    if (loginForm.exec() == QDialog::Accepted) {
        calendarChoiceDialog.resize(loginForm.size());
        calendarChoiceDialog.show();
        if (calendarChoiceDialog.exec() == QDialog::Accepted) {
            CalendarWidget calendarWidget(nullptr);
            QList<Calendar *> calendars = calendarChoiceDialog.getSelectedCalendars();
            // creates a new ConnectionManager for each Calendar selected
            for (auto & calendar : calendars) {
                ConnectionManager *connectionManager_ = new ConnectionManager(connectionManager->getUsername(),
                                                                              connectionManager->getPassword());
                connectionManager_->setCalendar(calendar);
                calendarWidget.addConnectionManager(connectionManager_);
            }
            // calendarWidget setups
            calendarWidget.setupConnection();
            calendarWidget.setupTimer();
            calendarWidget.show();
            return QApplication::exec();
        }
    }
    return 0;
}
