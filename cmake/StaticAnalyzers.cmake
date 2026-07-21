# SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
# SPDX-License-Identifier: GPL-3.0-or-later

# Options controlling static analysis integrations.
option(HELLO_ENABLE_CLANG_TIDY "Run clang-tidy alongside the compiler" ON)
option(HELLO_ENABLE_CPPCHECK "Run cppcheck alongside the compiler" OFF)
option(HELLO_ENABLE_IWYU "Run include-what-you-use alongside the compiler" OFF)

# Wires up clang-tidy / cppcheck / include-what-you-use if the relevant
# option is enabled and the tool is available on the system. Existing
# user-provided CMAKE_* values are preserved.
function(enable_static_analyzers target_name)
    if(HELLO_ENABLE_CLANG_TIDY AND NOT CMAKE_CXX_CLANG_TIDY)
        find_program(CLANG_TIDY_BIN NAMES clang-tidy)
        if(CLANG_TIDY_BIN)
            message(STATUS "Found clang-tidy: ${CLANG_TIDY_BIN}")
            set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_BIN}" PARENT_SCOPE)
        else()
            message(STATUS "clang-tidy not found, skipping")
        endif()
    endif()

    if(HELLO_ENABLE_CPPCHECK AND NOT CMAKE_CXX_CPPCHECK)
        find_program(CPPCHECK_BIN NAMES cppcheck)
        if(CPPCHECK_BIN)
            message(STATUS "Found cppcheck: ${CPPCHECK_BIN}")
            set(CMAKE_CXX_CPPCHECK
                "${CPPCHECK_BIN};--inline-suppr;--quiet" PARENT_SCOPE)
        else()
            message(STATUS "cppcheck not found, skipping")
        endif()
    endif()

    if(HELLO_ENABLE_IWYU AND NOT CMAKE_CXX_INCLUDE_WHAT_YOU_USE)
        find_program(IWYU_BIN NAMES include-what-you-use iwyu)
        if(IWYU_BIN)
            message(STATUS "Found include-what-you-use: ${IWYU_BIN}")
            set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "${IWYU_BIN}" PARENT_SCOPE)
        else()
            message(STATUS "include-what-you-use not found, skipping")
        endif()
    endif()
endfunction()
