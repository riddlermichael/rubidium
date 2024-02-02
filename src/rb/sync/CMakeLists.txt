set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

if(DEFINED CMAKE_USE_PTHREADS_INIT)
    set(RB_USE_PTHREADS 1)
    set(RB_USE_WIN32_THREADS "(-1)")
    message(STATUS ">>> Thread model: pthreads")
elseif(DEFINED CMAKE_USE_WIN32_THREADS_INIT)
    set(RB_USE_PTHREADS "(-1)")
    set(RB_USE_WIN32_THREADS 1)
    message(STATUS ">>> Thread model: win32")
else()
    message(FATAL_ERROR "Unsupported thread library")
endif()

set(FEATURE_FILE "${GENERATED_INCLUDE_DIR}/rb/sync.hpp")
file(WRITE ${FEATURE_FILE} "/*
 * This file is auto-generated.
 * Do not edit this manually.
 * Do not include this directly.
 */\n\n")

file(APPEND ${FEATURE_FILE} "#define RB_USE_PTHREADS ${RB_USE_PTHREADS}\n")
file(APPEND ${FEATURE_FILE} "#define RB_USE_WIN32_THREADS ${RB_USE_WIN32_THREADS}\n")

add_cxx_module(sync ${RB_ROOT_DIR})
set(HEADERS "${HEADERS}" PARENT_SCOPE)
set(SOURCES "${SOURCES}" PARENT_SCOPE)
