# SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

option(MO_ENABLE_COVERAGE "Enable code coverage instrumentation (GCC/Clang)" OFF)

# Enables --coverage instrumentation on GCC and Clang builds. No-op on
# other compilers.
function(enable_coverage target_name)
    if(NOT MO_ENABLE_COVERAGE)
        return()
    endif()

    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(WARNING
            "Coverage instrumentation only supported on GCC/Clang "
            "(got ${CMAKE_CXX_COMPILER_ID}); skipping.")
        return()
    endif()

    add_library(mo_coverage INTERFACE)
    target_compile_options(mo_coverage INTERFACE
        --coverage
        -fprofile-arcs
        -ftest-coverage
    )
    target_link_options(mo_coverage INTERFACE
        --coverage
    )
    target_link_libraries(${target_name} PRIVATE mo_coverage)
endfunction()
