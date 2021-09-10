#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::LabsSharedImage" for configuration "Debug"
set_property(TARGET Qt6::LabsSharedImage APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(Qt6::LabsSharedImage PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/Qt6LabsSharedImaged.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/Qt6LabsSharedImaged.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS Qt6::LabsSharedImage )
list(APPEND _IMPORT_CHECK_FILES_FOR_Qt6::LabsSharedImage "${_IMPORT_PREFIX}/lib/Qt6LabsSharedImaged.lib" "${_IMPORT_PREFIX}/bin/Qt6LabsSharedImaged.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
