#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "hlp::hlp" for configuration "Release"
set_property(TARGET hlp::hlp APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(hlp::hlp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libhlp.a"
  )

list(APPEND _cmake_import_check_targets hlp::hlp )
list(APPEND _cmake_import_check_files_for_hlp::hlp "${_IMPORT_PREFIX}/lib/libhlp.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
