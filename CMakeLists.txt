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
include(Utils)

# compile options and sanitizers' settings act on current directory and below
include(Sanitizers)

option(BUILD_TESTING "Build tests")
if(BUILD_TESTING)
    include(CTest)
    enable_testing()
endif()

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
if(NOT DEFINED CMAKE_CXX_VISIBILITY_PRESET AND NOT DEFINED CMAKE_VISIBILITY_INLINES_HIDDEN)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
endif()

add_subdirectory(src)

if(IS_TOP_LEVEL_PROJECT AND BUILD_TESTING)
    set(CATCH_INSTALL_DOCS OFF CACHE BOOL "Catch2's documentation won't be included in the installation")
    add_subdirectory(tests)
endif()
