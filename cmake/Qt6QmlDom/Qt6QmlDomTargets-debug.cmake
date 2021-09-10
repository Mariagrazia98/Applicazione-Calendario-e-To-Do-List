#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::QmlDom" for configuration "Debug"
set_property(TARGET Qt6::QmlDom APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(Qt6::QmlDom PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/Qt6QmlDomd.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS Qt6::QmlDom )
list(APPEND _IMPORT_CHECK_FILES_FOR_Qt6::QmlDom "${_IMPORT_PREFIX}/lib/Qt6QmlDomd.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
