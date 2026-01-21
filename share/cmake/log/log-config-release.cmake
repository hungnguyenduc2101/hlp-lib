#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "hlp::log" for configuration "Release"
set_property(TARGET hlp::log APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(hlp::log PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/liblog.a"
  )

list(APPEND _cmake_import_check_targets hlp::log )
list(APPEND _cmake_import_check_files_for_hlp::log "${_IMPORT_PREFIX}/lib/liblog.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
