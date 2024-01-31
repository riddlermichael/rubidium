include(compilers)

option(USE_ASAN "Enable address sanitizer")
option(USE_HWASAN "Enable hardware-assisted address sanitizer")
option(USE_TSAN "Enable thread sanitizer")

option(USE_UBSAN "Enable undefined behavior sanitizer")
if(${USE_UBSAN} AND ${COMPILER_GCC}) # gcc itself doesn't define this macro yet
    add_compile_definitions(__SANITIZE_UNDEFINED__) # really should be global
endif()

if(${COMPILER_CLANG})
    option(USE_MSAN "Enable memory sanitizer")
elseif(${USE_MSAN})
    message(FATAL_ERROR "Memory sanitizer is supported only in clang")
else()
    set(USE_MSAN OFF CACHE BOOL "Enable memory sanitizer")
endif()

if((${USE_ASAN} AND ${USE_HWASAN})
    OR ((${USE_ASAN} OR ${USE_HWASAN}) AND (${USE_TSAN} OR ${USE_MSAN}))
    OR (${USE_TSAN} AND ${USE_MSAN}))
    message(FATAL_ERROR "These sanitizers cannot be used simultaneously")
endif()
# in other words, usable configurations are:
# UB + [A | HWA | T | M]

if(${COMPILER_MSVC_LIKE})

    if(${USE_ASAN} AND MSVC_VERSION LESS 1928)
        message(FATAL_ERROR "Address sanitizer is not supported")
    endif()

    if(${USE_HWASAN})
        message(FATAL_ERROR "Hardware-assisted address sanitizer is not supported")
    endif()

    if(${USE_TSAN})
        message(FATAL_ERROR "Thread sanitizer is not supported")
    endif()

    if(${USE_UBSAN})
        message(FATAL_ERROR "Undefined behavior sanitizer is not supported")
    endif()

    function(use_sanitizers target)
        if(${USE_ASAN})
            if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" AND ${COMPILER_CLANG_MSVC})
                message(FATAL_ERROR "AddressSanitizer doesn't support linking with debug runtime libraries yet")
            endif()

            target_compile_options(${target}
                PUBLIC /fsanitize=address
                PUBLIC /Zi)
            target_compile_options(${target} PRIVATE /wd5072) # disable "ASAN enabled without debug information emission"

            target_link_options(${target}
                PRIVATE /ignore:4302)
            if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
                target_link_options(${target}
                    PUBLIC clang_rt.asan_dynamic-x86_64.lib
                    PUBLIC clang_rt.asan_dynamic_runtime_thunk-x86_64.lib)
            else()
                target_link_options(${target}
                    PUBLIC clang_rt.asan_dbg_dynamic-x86_64.lib
                    PUBLIC clang_rt.asan_dbg_dynamic_runtime_thunk-x86_64.lib)
            endif()
        endif()
    endfunction()

else()

    function(use_sanitizers target)
        if(${USE_ASAN} OR ${USE_TSAN} OR ${USE_MSAN} OR ${USE_UBSAN})
            target_compile_options(${target}
                PUBLIC -g
                PUBLIC -fno-omit-frame-pointer
                PUBLIC -fno-optimize-sibling-calls)
            target_link_options(${target} PUBLIC -g)
        endif()

        if(${USE_ASAN})
            target_compile_options(${target}
                PUBLIC -fsanitize=address,pointer-compare,pointer-subtract
                PUBLIC -fno-common)
            target_link_options(${target} PUBLIC -fsanitize=address,pointer-compare,pointer-subtract)
        endif()

        if(${USE_TSAN})
            target_compile_options(${target} PUBLIC -fsanitize=thread)
            target_link_options(${target} PUBLIC -fsanitize=thread)
        endif()

        if(${USE_HWASAN})
            target_compile_options(${target} PUBLIC -fsanitize=hwaddress)
            target_link_options(${target} PUBLIC -fsanitize=hwaddress)
        endif()

        if(${USE_MSAN})
            target_compile_options(${target}
                PUBLIC -fsanitize=memory
                PUBLIC -fsanitize-memory-use-after-dtor
                # 1 is slightly faster, 2 collects both allocation points and all intermediate stores
                PUBLIC -fsanitize-memory-track-origins=1
                PUBLIC -fPIE)
            target_link_options(${target}
                PUBLIC -fsanitize=memory
                PUBLIC -fsanitize-memory-track-origins=1
                PUBLIC -fPIE -pie)
        endif()

        if(${USE_UBSAN})
            target_compile_options(${target} PUBLIC -fsanitize=undefined,float-cast-overflow,float-divide-by-zero)
            target_link_options(${target} PUBLIC -fsanitize=undefined,float-cast-overflow,float-divide-by-zero)
            if(${COMPILER_CLANG})
                target_compile_options(${target} PUBLIC -fsanitize=local-bounds,integer,nullability)
                target_link_options(${target} PUBLIC -fsanitize=local-bounds,integer,nullability)
            endif()
            if(${COMPILER_GCC})
                target_compile_options(${target} PUBLIC -fsanitize=bounds-strict)
                target_link_options(${target} PUBLIC -fsanitize=bounds-strict)
            endif()
        endif()
    endfunction()

endif()

# Common option:
# halt_on_error=1: exit after first reported error
#
# ASAN_OPTIONS:
# debug: if set, prints some debugging information and does additional checks
# check_initialization_order: if set, attempts to catch initialization order issues
# detect_stack_use_after_return: enables stack-use-after-return checking at run-time
# print_stats: print various statistics after printing an error message or if atexit=1
# atexit: if set, prints ASan exit stats even after program terminates successfully
# strict_init_order: if true, assume that dynamic initializers can never access globals from other modules,
#   even if the latter are already initialized
# strict_string_checks: if true, check that string arguments are properly null-terminated
# detect_invalid_pointer_pairs: if non-zero, try to detect operations like <, <=, >, >= and - on invalid pointer pairs
#   (e.g. when pointers belong to different objects). The bigger the value the harder we try.
#   Using detect_invalid_pointer_pairs=1 detects invalid operation only when both pointers are non-null.
# halt_on_error: crash the program after printing the first error report.
#   The flag has effect only if code was compiled with -fsanitize-recover=address compile option.
#
# MSAN_OPTIONS=poison_in_dtor=1 for use-after-dtor
