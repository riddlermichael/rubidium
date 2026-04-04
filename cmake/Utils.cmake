include_guard(GLOBAL)

function(dump var)
    if(DEFINED ${var})
        message(STATUS ">>> ${var}: ${${var}}")
    else()
        message(STATUS ">>> ${var}: <null>")
    endif()
endfunction()

function(add_module MODULE ROOT)
    set(ONE_VALUE_ARGS HEADERS SOURCES)
    set(MULTI_VALUE_ARGS HEADER_EXTENSIONS SOURCE_EXTENSIONS)
    cmake_parse_arguments(arg "" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

    if(DEFINED arg_KEYWORDS_MISSING_VALUES)
        message(FATAL_ERROR
            "Invalid invocation of add_module: "
            "missing value for ${arg_KEYWORDS_MISSING_VALUES} keyword(s)")
    endif()

    if(DEFINED arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Invalid invocation of add_module: "
            "unknown argument(s) ${arg_UNPARSED_ARGUMENTS}")
    endif()

    set(MODULE_DIR "${ROOT}/${MODULE}")

    set(LOCAL_HEADERS)
    foreach(EXT IN LISTS arg_HEADER_EXTENSIONS)
        file(GLOB FILES CONFIGURE_DEPENDS "${MODULE_DIR}/*.${EXT}")
        list(APPEND LOCAL_HEADERS ${FILES})
    endforeach()

    set(LOCAL_SOURCES)
    foreach(EXT IN LISTS arg_SOURCE_EXTENSIONS)
        file(GLOB FILES CONFIGURE_DEPENDS "${MODULE_DIR}/*.${EXT}")
        list(APPEND LOCAL_SOURCES ${FILES})
    endforeach()

    set(${arg_HEADERS} ${LOCAL_HEADERS} PARENT_SCOPE)
    set(${arg_SOURCES} ${LOCAL_SOURCES} PARENT_SCOPE)
endfunction()

function(add_cxx_module MODULE ROOT)
    set(CXX_HEADERS)
    set(CXX_SOURCES)
    add_module("${MODULE}" "${ROOT}"
        HEADERS CXX_HEADERS
        SOURCES CXX_SOURCES
        HEADER_EXTENSIONS hpp
        SOURCE_EXTENSIONS cpp)
    set(HEADERS ${HEADERS} ${CXX_HEADERS} PARENT_SCOPE)
    set(SOURCES ${SOURCES} ${CXX_SOURCES} PARENT_SCOPE)
endfunction()
