# Public Repo CMakeLists.txt
cmake_minimum_required(VERSION 3.30)
project(hlp-lib)

# add_library(hlp-lib STATIC IMPORTED GLOBAL)
#
# set_target_properties(hlp-lib PROPERTIES
#   INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include"
#   IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/liblog.a" 
# )
# add_library(hlp::ndh ALIAS hlp-lib)


include("${CMAKE_CURRENT_LIST_DIR}/share/cmake/hlp/hlp-config.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/share/cmake/config/config-config.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/share/cmake/log/log-config.cmake")

get_target_property(_imported_configs hlp::hlp IMPORTED_CONFIGURATIONS)
get_target_property(_imported_configs hlp::config IMPORTED_CONFIGURATIONS)
get_target_property(_imported_configs hlp::log IMPORTED_CONFIGURATIONS)
foreach(_target hlp::hlp hlp::config hlp::log)
    set_target_properties(${_target} PROPERTIES IMPORTED_GLOBAL TRUE)
endforeach()
