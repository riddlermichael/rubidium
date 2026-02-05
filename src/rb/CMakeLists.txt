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

if(COMPILER_GCC)
    target_compile_options(${LIB} PRIVATE -fdiagnostics-color=always)

    target_compile_options(${LIB} PRIVATE -Wall)
    target_compile_options(${LIB} PRIVATE -Wextra)
    target_compile_options(${LIB} PRIVATE -pedantic)
    target_compile_options(${LIB} PRIVATE -Wshadow=local)

    target_compile_options(${LIB} PRIVATE -Wno-unknown-pragmas)
    target_compile_options(${LIB} PRIVATE -Wno-nonnull-compare)
    target_compile_options(${LIB} PRIVATE -Wno-parentheses)
    target_compile_options(${LIB} PRIVATE -fanalyzer -Wno-analyzer-use-of-uninitialized-value)
elseif(COMPILER_CLANG)
    target_compile_options(${LIB} PRIVATE -fcolor-diagnostics)

    target_compile_options(${LIB} PRIVATE -Wall)
    target_compile_options(${LIB} PRIVATE -Wextra)
    target_compile_options(${LIB} PRIVATE -pedantic)
    target_compile_options(${LIB} PRIVATE -Wthread-safety)
    target_compile_options(${LIB} PRIVATE -Wshadow -Wshadow-field -Wshadow-field-in-constructor-modified -Wshadow-uncaptured-local)

    target_compile_options(${LIB} PRIVATE -Wno-unknown-pragmas)
    target_compile_options(${LIB} PRIVATE -Wno-logical-op-parentheses)
    target_compile_options(${LIB} PRIVATE -Wno-language-extension-token)
    target_compile_options(${LIB} PRIVATE -Wno-gnu-statement-expression-from-macro-expansion)
    if(${COMPILER_CLANG_MSVC})
        target_compile_options(${LIB} PRIVATE /EHsc)
    endif()
elseif(COMPILER_MSVC)
    cmake_host_system_information(RESULT NUMBER_OF_LOGICAL_CORES QUERY NUMBER_OF_LOGICAL_CORES)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP${NUMBER_OF_LOGICAL_CORES}")

    target_compile_options(${LIB} PRIVATE /Wall)
    target_compile_options(${LIB} PRIVATE /EHsc)
    target_compile_options(${LIB} PRIVATE /analyze)
    # disable "unknown pragma" warnings
    target_compile_options(${LIB} PRIVATE /wd4068)
    # disable "exception-filter expression is the constant EXCEPTION_EXECUTE_HANDLER" warnings
    target_compile_options(${LIB} PRIVATE /wd6320)
else()
    message(FATAL_ERROR "Unsupported compiler")
endif()

use_sanitizers(${LIB})

if(IS_TOP_LEVEL_PROJECT AND NOT CMAKE_SKIP_INSTALL_RULES)
    include(Deploy)
endif()
