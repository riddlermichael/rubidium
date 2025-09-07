set(RB_HOME ${ROOT_DIR} CACHE PATH "rb home" FORCE)
set(RB_ROOT_DIR "${RB_HOME}/rb" CACHE PATH "rb root" FORCE)
set(PRODUCT_NAME rb)

option(BUILD_SHARED_LIBS "Build shared libraries")
# in rare cases an user may want to build some libs in other way than BUILD_SHARED_LIBS determines
# (for example all used libs are shared, but Rubidium should be built as static)
if(DEFINED RB_USE_SHARED_LIBS)
    set(BUILD_SHARED_LIBS "${RB_USE_SHARED_LIBS}")
endif()

# generate version file
set(GENERATED_INCLUDE_DIR "${CMAKE_BINARY_DIR}/include")
set(VERSION_FILE "${GENERATED_INCLUDE_DIR}/${PRODUCT_NAME}/${PRODUCT_NAME}_version.hpp")
configure_file(version.hpp.in "${VERSION_FILE}")

# collect features
include(Features)

# collect sources
set(MODULES
    containers
    core
    core/error
    core/iter
    core/memory
    core/meta
    core/slices
    core/str
    core/traits
    ext
    fmt
    ranges
    time)
set(HEADERS)
set(SOURCES)
foreach(X ${MODULES})
    add_cxx_module(${X} ${RB_ROOT_DIR})
endforeach()
add_subdirectory(sync)

source_group(TREE "${ROOT_DIR}" FILES ${HEADERS} ${SOURCES})

# create lib
set(INCLUDE_DIRS
    $<BUILD_INTERFACE:${RB_HOME}>
    $<BUILD_INTERFACE:${GENERATED_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

if(${BUILD_SHARED_LIBS})
    set(LIB ${PRODUCT_NAME}-shared)
else()
    set(LIB ${PRODUCT_NAME})
endif()

add_library(${LIB} ${HEADERS} ${SOURCES})
add_library(Rb::Rb ALIAS ${LIB})
target_compile_features(${LIB} PUBLIC cxx_std_17)
target_compile_definitions(${LIB}
    PUBLIC $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:RB_STATIC>
    PRIVATE RB_EXPORTS)
target_include_directories(${LIB}
    PUBLIC $<BUILD_INTERFACE:${INCLUDE_DIRS}>
    PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
set(OUTPUT_DIR "${CMAKE_BINARY_DIR}")
set_target_properties(${LIB} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR} # .lib/.a for static, .lib for shared on Windows
    DEBUG_POSTFIX d
    LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR} # .so
    RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR} # .dll
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION})
set_target_properties(${LIB} PROPERTIES EXPORT_NAME Rb)

if(WIN32)
    target_compile_definitions(${LIB} PRIVATE WIN32_LEAN_AND_MEAN)
    if(COMPILER_MSVC_LIKE)
        target_compile_definitions(${LIB} PUBLIC NOMINMAX)
    endif()
elseif(UNIX)
    target_compile_definitions(${LIB} PRIVATE _GNU_SOURCE)
endif()

use_sanitizers(${LIB})

if(IS_TOP_LEVEL_PROJECT AND NOT CMAKE_SKIP_INSTALL_RULES)
    include(Deploy)
endif()
