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

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
add_subdirectory(sync)

source_group(TREE "${ROOT_DIR}" FILES ${HEADERS} ${SOURCES})

# create lib
set(INCLUDE_DIRS
    $<BUILD_INTERFACE:${RB_HOME}>
    $<BUILD_INTERFACE:${GENERATED_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

if(BUILD_SHARED_LIBS)
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
target_link_libraries(${LIB} PUBLIC Threads::Threads)

set(OUTPUT_DIR "${CMAKE_BINARY_DIR}")
set_target_properties(${LIB} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR} # .lib/.a for static, .lib for shared on Windows
    DEBUG_POSTFIX d
    LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR} # .so
    RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR} # .dll
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION})
set_target_properties(${LIB} PROPERTIES EXPORT_NAME Rb)

# compile definitions
if(WIN32)
    target_compile_definitions(${LIB} PRIVATE WIN32_LEAN_AND_MEAN)
    target_compile_definitions(${LIB} PUBLIC NOMINMAX)
elseif(UNIX)
    target_compile_definitions(${LIB} PRIVATE _GNU_SOURCE)
endif()

# compile options
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24")
    set(CMAKE_COLOR_DIAGNOSTICS ON)
else()
    if(COMPILER_GCC)
        target_compile_options(${LIB} PRIVATE -fdiagnostics-color=always)
    elseif(COMPILER_CLANG)
        target_compile_options(${LIB} PRIVATE -fcolor-diagnostics)
    endif()
endif()

if(COMPILER_GCC)
    target_compile_options(${LIB} PRIVATE -pedantic)
    target_compile_options(${LIB} PRIVATE -Wall)
    target_compile_options(${LIB} PRIVATE -Wextra)
    target_compile_options(${LIB} PRIVATE -Wpadded)
    target_compile_options(${LIB} PRIVATE -Wshadow=local)

    target_compile_options(${LIB} PRIVATE -Wno-nonnull-compare)
    target_compile_options(${LIB} PRIVATE -Wno-parentheses)
    target_compile_options(${LIB} PRIVATE -Wno-unknown-pragmas)

    target_compile_options(${LIB} PRIVATE -fanalyzer -Wno-analyzer-use-of-uninitialized-value)
    target_compile_options(${LIB} PRIVATE -fstrict-enums)
elseif(COMPILER_CLANG)
    target_compile_options(${LIB} PRIVATE -pedantic)
    target_compile_options(${LIB} PRIVATE -Wall)
    target_compile_options(${LIB} PRIVATE -Wextra)
    target_compile_options(${LIB} PRIVATE -Wpadded)
    target_compile_options(${LIB} PRIVATE -Wshadow -Wshadow-uncaptured-local)
    target_compile_options(${LIB} PRIVATE -Wshadow-field -Wshadow-field-in-constructor-modified)
    target_compile_options(${LIB} PRIVATE -Wthread-safety)

    target_compile_options(${LIB} PRIVATE -Wno-gnu-statement-expression-from-macro-expansion)
    target_compile_options(${LIB} PRIVATE -Wno-language-extension-token)
    target_compile_options(${LIB} PRIVATE -Wno-logical-op-parentheses)
    target_compile_options(${LIB} PRIVATE -Wno-unknown-pragmas)

    if(COMPILER_CLANG_MSVC)
        target_compile_options(${LIB} PRIVATE /EHsc)
    endif()
elseif(COMPILER_MSVC)
    cmake_host_system_information(RESULT NUMBER_OF_LOGICAL_CORES QUERY NUMBER_OF_LOGICAL_CORES)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP${NUMBER_OF_LOGICAL_CORES}")

    target_compile_options(${LIB} PRIVATE /analyze)
    target_compile_options(${LIB} PRIVATE /EHsc)
    target_compile_options(${LIB} PRIVATE /Wall)

    target_compile_options(${LIB} PRIVATE /wd4068) # unknown pragma
    target_compile_options(${LIB} PRIVATE /wd4296) # expression is always false
    target_compile_options(${LIB} PRIVATE /wd4365) # signed/unsigned mismatch
    target_compile_options(${LIB} PRIVATE /wd4514) # unreferenced inline function has been removed
    target_compile_options(${LIB} PRIVATE /wd4582) # constructor is not implicitly called
    target_compile_options(${LIB} PRIVATE /wd4623) # default constructor was implicitly defined as deleted
    target_compile_options(${LIB} PRIVATE /wd4625) # copy constructor was implicitly defined as deleted
    target_compile_options(${LIB} PRIVATE /wd4626) # assignment operator was implicitly defined as deleted
    target_compile_options(${LIB} PRIVATE /wd4710) # function not inlined
    target_compile_options(${LIB} PRIVATE /wd4711) # function selected for inline expansion
    target_compile_options(${LIB} PRIVATE /wd4800) # implicit conversion to bool
    target_compile_options(${LIB} PRIVATE /wd4866) # compiler may not enforce left-to-right evaluation order for call to operator[]
    target_compile_options(${LIB} PRIVATE /wd4868) # compiler may not enforce left-to-right evaluation order in braced initializer list
    target_compile_options(${LIB} PRIVATE /wd5026) # move constructor was implicitly defined as deleted
    target_compile_options(${LIB} PRIVATE /wd5027) # move assignment operator was implicitly defined as deleted
    target_compile_options(${LIB} PRIVATE /wd5045) # compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
    target_compile_options(${LIB} PRIVATE /wd6320) # exception-filter expression is the constant EXCEPTION_EXECUTE_HANDLER
else()
    message(FATAL_ERROR "Unsupported compiler")
endif()

use_sanitizers(${LIB})

if(PROJECT_IS_TOP_LEVEL AND NOT CMAKE_SKIP_INSTALL_RULES)
    include(Deploy)
endif()
