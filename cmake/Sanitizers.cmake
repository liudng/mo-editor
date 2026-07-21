# SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
# SPDX-License-Identifier: GPL-3.0-or-later

# Sanitizer selection. Set HELLO_ENABLE_SANITIZER to one of:
#   asan, tsan, ubsan, msan, lsan
# Note: AddressSanitizer and ThreadSanitizer are mutually exclusive; only
# one sanitizer may be enabled at a time through this option.
set(HELLO_ENABLE_SANITIZER "" CACHE STRING
    "Enable a sanitizer (one of: asan, tsan, ubsan, msan, lsan)")

function(enable_sanitizers target_name)
    if(NOT HELLO_ENABLE_SANITIZER)
        return()
    endif()

    # Common option that improves stack traces in sanitizer reports.
    set(SANITIZER_COMMON_FLAGS -fno-omit-frame-pointer)

    if(HELLO_ENABLE_SANITIZER STREQUAL "asan")
        # AddressSanitizer. ASan and TSan are mutually exclusive.
        set(SANITIZER_FLAGS -fsanitize=address)
        set(ENV{ASAN_OPTIONS}
            "detect_leaks=1:abort_on_error=1:print_summary=1:detect_stack_use_after_return=1")
        message(STATUS "Enabling AddressSanitizer (ASAN_OPTIONS hint set)")
    elseif(HELLO_ENABLE_SANITIZER STREQUAL "tsan")
        # ThreadSanitizer. Mutually exclusive with AddressSanitizer.
        set(SANITIZER_FLAGS -fsanitize=thread)
        message(STATUS "Enabling ThreadSanitizer (mutually exclusive with ASan)")
    elseif(HELLO_ENABLE_SANITIZER STREQUAL "ubsan")
        # UndefinedBehaviorSanitizer.
        set(SANITIZER_FLAGS -fsanitize=undefined)
        message(STATUS "Enabling UndefinedBehaviorSanitizer")
    elseif(HELLO_ENABLE_SANITIZER STREQUAL "msan")
        # MemorySanitizer (Clang only).
        if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            message(FATAL_ERROR
                "MemorySanitizer is only supported by Clang (got ${CMAKE_CXX_COMPILER_ID})")
        endif()
        set(SANITIZER_FLAGS -fsanitize=memory)
        message(STATUS "Enabling MemorySanitizer (Clang only)")
    elseif(HELLO_ENABLE_SANITIZER STREQUAL "lsan")
        # LeakSanitizer.
        set(SANITIZER_FLAGS -fsanitize=leak)
        message(STATUS "Enabling LeakSanitizer")
    else()
        message(FATAL_ERROR
            "Unknown sanitizer '${HELLO_ENABLE_SANITIZER}' "
            "(expected: asan, tsan, ubsan, msan, lsan)")
    endif()

    target_compile_options(${target_name} PRIVATE
        ${SANITIZER_FLAGS}
        ${SANITIZER_COMMON_FLAGS}
    )
    target_link_options(${target_name} PRIVATE
        ${SANITIZER_FLAGS}
        ${SANITIZER_COMMON_FLAGS}
    )
endfunction()
