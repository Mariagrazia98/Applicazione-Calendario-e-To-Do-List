set(qtquickcontrols2basicstyleplugin_FOUND FALSE)

# note: _third_party_deps example: "ICU\\;FALSE\\;1.0\\;i18n uc data;ZLIB\\;FALSE\\;\\;"
set(__qt_qtquickcontrols2basicstyleplugin_third_party_deps "")

foreach(__qt_qtquickcontrols2basicstyleplugin_target_dep ${__qt_qtquickcontrols2basicstyleplugin_third_party_deps})
    list(GET __qt_qtquickcontrols2basicstyleplugin_target_dep 0 __qt_qtquickcontrols2basicstyleplugin_pkg)
    list(GET __qt_qtquickcontrols2basicstyleplugin_target_dep 1 __qt_qtquickcontrols2basicstyleplugin_is_optional)
    list(GET __qt_qtquickcontrols2basicstyleplugin_target_dep 2 __qt_qtquickcontrols2basicstyleplugin_version)
    list(GET __qt_qtquickcontrols2basicstyleplugin_target_dep 3 __qt_qtquickcontrols2basicstyleplugin_components)
    list(GET __qt_qtquickcontrols2basicstyleplugin_target_dep 4 __qt_qtquickcontrols2basicstyleplugin_optional_components)
    set(__qt_qtquickcontrols2basicstyleplugin_find_package_args "${__qt_qtquickcontrols2basicstyleplugin_pkg}")
    if(__qt_qtquickcontrols2basicstyleplugin_version)
        list(APPEND __qt_qtquickcontrols2basicstyleplugin_find_package_args "${__qt_qtquickcontrols2basicstyleplugin_version}")
    endif()
    if(__qt_qtquickcontrols2basicstyleplugin_components)
        string(REPLACE " " ";" __qt_qtquickcontrols2basicstyleplugin_components "${__qt_qtquickcontrols2basicstyleplugin_components}")
        list(APPEND __qt_qtquickcontrols2basicstyleplugin_find_package_args COMPONENTS ${__qt_qtquickcontrols2basicstyleplugin_components})
    endif()
    if(__qt_qtquickcontrols2basicstyleplugin_optional_components)
        string(REPLACE " " ";" __qt_qtquickcontrols2basicstyleplugin_optional_components "${__qt_qtquickcontrols2basicstyleplugin_optional_components}")
        list(APPEND __qt_qtquickcontrols2basicstyleplugin_find_package_args OPTIONAL_COMPONENTS ${__qt_qtquickcontrols2basicstyleplugin_optional_components})
    endif()

    if(__qt_qtquickcontrols2basicstyleplugin_is_optional)
        if(${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY)
            list(APPEND __qt_qtquickcontrols2basicstyleplugin_find_package_args QUIET)
        endif()
        find_package(${__qt_qtquickcontrols2basicstyleplugin_find_package_args})
    else()
        find_dependency(${__qt_qtquickcontrols2basicstyleplugin_find_package_args})
    endif()
endforeach()

set(__qt_use_no_default_path_for_qt_packages "NO_DEFAULT_PATH")
if(QT_DISABLE_NO_DEFAULT_PATH_IN_QT_PACKAGES)
    set(__qt_use_no_default_path_for_qt_packages "")
endif()

# note: target_deps example: "Qt6Core\;5.12.0;Qt6Gui\;5.12.0"
set(__qt_qtquickcontrols2basicstyleplugin_target_deps "Qt6Core\;6.1.3;Qt6Gui\;6.1.3;Qt6Qml\;6.1.3;Qt6Quick\;6.1.3")
foreach(__qt_qtquickcontrols2basicstyleplugin_target_dep ${__qt_qtquickcontrols2basicstyleplugin_target_deps})
    list(GET __qt_qtquickcontrols2basicstyleplugin_target_dep 0 __qt_qtquickcontrols2basicstyleplugin_pkg)
    list(GET __qt_qtquickcontrols2basicstyleplugin_target_dep 1 __qt_qtquickcontrols2basicstyleplugin_version)

    if (NOT ${__qt_qtquickcontrols2basicstyleplugin_pkg}_FOUND)
        find_dependency(${__qt_qtquickcontrols2basicstyleplugin_pkg} ${__qt_qtquickcontrols2basicstyleplugin_version}
            PATHS
                ${CMAKE_CURRENT_LIST_DIR}/../..
                ${_qt_additional_packages_prefix_path}
                ${_qt_additional_packages_prefix_path_env}
                ${QT_EXAMPLES_CMAKE_PREFIX_PATH}
            ${__qt_use_no_default_path_for_qt_packages}
        )
    endif()
endforeach()

set(qtquickcontrols2basicstyleplugin_FOUND TRUE)
