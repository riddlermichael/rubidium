function(dump var)
    if(DEFINED ${var})
        message(STATUS ">>> ${var}: ${${var}}")
    else()
        message(STATUS ">>> ${var}: <null>")
    endif()
endfunction()

function(add_module MODULE ROOT)
    set(OPTIONS)
    set(ONE_VALUE_ARGS HEADERS SOURCES)
    set(MULTI_VALUE_ARGS HEADER_EXTENSIONS SOURCE_EXTENSIONS)
    cmake_parse_arguments(arg "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

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

    if(DEFINED arg_HEADERS)
        set(HEADERS "${arg_HEADERS}")
    else()
        set(HEADERS "HEADERS")
    endif()

    if(DEFINED arg_SOURCES)
        set(SOURCES "${arg_SOURCES}")
    else()
        set(SOURCES "SOURCES")
    endif()

    string(REPLACE "-" "/" PATH ${MODULE})

    set(MODULE_HEADERS)
    foreach(EXT IN LISTS arg_HEADER_EXTENSIONS)
        file(GLOB EXT_HEADERS "${ROOT}/${PATH}/*.${EXT}")
        list(APPEND MODULE_HEADERS ${EXT_HEADERS})
    endforeach()

    set(MODULE_SOURCES)
    foreach(EXT IN LISTS arg_SOURCE_EXTENSIONS)
        file(GLOB EXT_SOURCES "${ROOT}/${PATH}/*.${EXT}")
        list(APPEND MODULE_SOURCES ${EXT_SOURCES})
    endforeach()

    set(${HEADERS} ${${HEADERS}} ${MODULE_HEADERS} PARENT_SCOPE)
    set(${SOURCES} ${${SOURCES}} ${MODULE_SOURCES} PARENT_SCOPE)
endfunction()

function(add_c_module MODULE ROOT)
    set(C_HEADERS)
    set(C_SOURCES)
    add_module("${MODULE}" "${ROOT}"
        HEADERS C_HEADERS
        SOURCES C_SOURCES
        HEADER_EXTENSIONS h
        SOURCE_EXTENSIONS c)
    set(HEADERS ${HEADERS};${C_HEADERS} PARENT_SCOPE)
    set(SOURCES ${SOURCES};${C_SOURCES} PARENT_SCOPE)
endfunction()

function(add_cxx_module MODULE ROOT)
    set(CXX_HEADERS)
    set(CXX_SOURCES)
    add_module("${MODULE}" "${ROOT}"
        HEADERS CXX_HEADERS
        SOURCES CXX_SOURCES
        HEADER_EXTENSIONS hpp
        SOURCE_EXTENSIONS cpp)
    set(HEADERS ${HEADERS};${CXX_HEADERS} PARENT_SCOPE)
    set(SOURCES ${SOURCES};${CXX_SOURCES} PARENT_SCOPE)
endfunction()
