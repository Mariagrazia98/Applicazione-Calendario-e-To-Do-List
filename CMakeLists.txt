cmake_minimum_required(VERSION 3.17)
project(Applicazione-Calendario-e-To-Do-List-all-in--one)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_VERSION 6)
set(REQUIRED_LIBS Core Gui Widgets Network Xml Concurrent )
set(REQUIRED_LIBS_QUALIFIED Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network Qt6::Xml Qt6::Concurrent)

set(CMAKE_PREFIX_PATH "C:/Qt/6.1.3/msvc2019_64/lib/cmake")
#set(CMAKE_PREFIX_PATH {PROJECT_NAME}/cmake)



add_executable(${PROJECT_NAME} main.cpp src.qrc
        View/loginform.h View/loginform.cpp
        View/calendarwidget.h View/calendarwidget.cpp
        View/customcalendarwidget.cpp View/customcalendarwidget.h
        View/CalendarObjectForm.h View/CalendarObjectForm.cpp View/CalendarObjectForm.ui
        Model/calendarobject.cpp Model/calendarobject.h
        Model/calendarevent.cpp Model/calendarevent.h
        View/calendarobjectwidget.h View/calendarobjectwidget.cpp View/calendarobjectwidget.ui
        Controller/connectionManager.cpp Controller/connectionManager.h Model/calendartodo.cpp
        Model/calendartodo.h View/calendarchoicedialog.cpp
        View/calendarchoicedialog.h
        Model/calendar.cpp Model/calendar.h View/eliminationtaskdialog.cpp View/eliminationtaskdialog.h
        View/customcalendarwidget.cpp View/customcalendarwidget.h
        View/eliminationtaskdialog.cpp View/eliminationtaskdialog.h
        View/sharecalendarform.cpp View/sharecalendarform.h
        Controller/utils.h)

if (NOT CMAKE_PREFIX_PATH)
    message(WARNING "CMAKE_PREFIX_PATH is not defined, you may need to set it "
            "(-DCMAKE_PREFIX_PATH=\"path/to/Qt/lib/cmake\" or -DCMAKE_PREFIX_PATH=/usr/include/{host}/qt{version}/ on Ubuntu)")
endif ()

find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)
target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED})
if (WIN32)
    set(DEBUG_SUFFIX)
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB ${REQUIRED_LIBS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt${QT_VERSION}${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
