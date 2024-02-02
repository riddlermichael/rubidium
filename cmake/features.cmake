set(FEATURE_FILE "${GENERATED_INCLUDE_DIR}/rb/rb_features.hpp")

file(WRITE ${FEATURE_FILE} "/*
 * This file is auto-generated.
 * Do not edit this file manually.
 * Do not include this file directly, use #include <rb/core/features.hpp> instead.
 */\n\n")

include(CMakeDependentOption)

# regular features
option(FULL_SOURCE_LOCATION_INFO "Print full source location info" ON)

set(FEATURES
    FULL_SOURCE_LOCATION_INFO)

foreach(FEATURE ${FEATURES})
    if(${FEATURE})
        set(VALUE 1)
    else()
        set(VALUE "(-1)")
    endif()
    file(APPEND ${FEATURE_FILE} "#define RB_FEATURE_${FEATURE} ${VALUE}\n")
    dump(${FEATURE})
endforeach()
