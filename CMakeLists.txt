cmake_minimum_required(VERSION 3.15)

# from CMake 3.21 there is PROJECT_IS_TOP_LEVEL
if(NOT DEFINED PROJECT_NAME)
    set(IS_TOP_LEVEL_PROJECT ON)
else()
    set(IS_TOP_LEVEL_PROJECT OFF)
endif()

set(VERSION)
file(STRINGS VERSION VERSION)

project(rubidium
    DESCRIPTION "Rubidium C++ library"
    LANGUAGES CXX
    VERSION ${VERSION})

set(MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${MODULE_PATH}")
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" CACHE INTERNAL "CMake module path" FORCE)
include(utils)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install" CACHE PATH "Install prefix" FORCE)
endif()

# **MUST** be placed before **ANY** `add_subdirectory`
include(CTest)
dump(BUILD_TESTING)

add_subdirectory(src)

if(${IS_TOP_LEVEL_PROJECT} AND ${BUILD_TESTING})
    set(CATCH_INSTALL_DOCS OFF CACHE BOOL "Catch2's documentation won't be included in the installation")
    add_subdirectory(tests)
endif()
