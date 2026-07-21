# SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
# SPDX-License-Identifier: GPL-3.0-or-later

option(HELLO_ENABLE_COVERAGE "Enable code coverage instrumentation (GCC/Clang)" OFF)

# Enables --coverage instrumentation on GCC and Clang builds. No-op on
# other compilers.
function(enable_coverage target_name)
    if(NOT HELLO_ENABLE_COVERAGE)
        return()
    endif()

    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(WARNING
            "Coverage instrumentation only supported on GCC/Clang "
            "(got ${CMAKE_CXX_COMPILER_ID}); skipping.")
        return()
    endif()

    add_library(hello_coverage INTERFACE)
    target_compile_options(hello_coverage INTERFACE
        --coverage
        -fprofile-arcs
        -ftest-coverage
    )
    target_link_options(hello_coverage INTERFACE
        --coverage
    )
    target_link_libraries(${target_name} PRIVATE hello_coverage)
endfunction()
