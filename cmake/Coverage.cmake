# SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

option(MO_ENABLE_COVERAGE "Enable code coverage instrumentation (GCC/Clang)" OFF)

# Enables --coverage instrumentation on GCC and Clang builds. No-op on
# other compilers.
#
# When MO_ENABLE_COVERAGE is ON, coverage flags are applied globally via
# add_compile_options / link_libraries so that every target in the project
# (libraries, executables, tests) is instrumented. This ensures .gcda files
# are generated for all source files, not just those in explicitly opted-in
# targets.
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

    target_compile_options(${target_name} PRIVATE
        --coverage
        -fprofile-arcs
        -ftest-coverage
    )
    target_link_options(${target_name} PRIVATE --coverage)
endfunction()

# Global coverage instrumentation: when MO_ENABLE_COVERAGE is ON, apply
# --coverage to all targets created after this point. This is called from
# the top-level CMakeLists.txt after the option is defined.
macro(mo_enable_global_coverage)
    if(MO_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(STATUS "Code coverage instrumentation enabled (--coverage)")
        add_compile_options(--coverage -fprofile-arcs -ftest-coverage)
        link_libraries(--coverage)
    endif()
endmacro()

