# SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
# SPDX-License-Identifier: GPL-3.0-or-later

# Applies a strict modern warning set to the given target.
function(set_project_warnings target_name)
    set(GCC_WARNINGS
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        -Wimplicit-fallthrough
        -Werror=format-security
    )

    # GCC-only diagnostics.
    set(GCC_ONLY_WARNINGS
        -Wduplicated-cond
        -Wduplicated-branches
        -Wlogical-op
        -Wuseless-cast
    )

    # Clang-only diagnostics. We target C++20, so silence the legacy
    # C++98 compatibility pedantry.
    set(CLANG_ONLY_WARNINGS
        -Wno-c++98-compat-pedantic
    )

    add_library(hello_project_warnings INTERFACE)
    target_compile_options(hello_project_warnings INTERFACE
        ${GCC_WARNINGS}
        $<$<CXX_COMPILER_ID:GNU>:${GCC_ONLY_WARNINGS}>
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:${CLANG_ONLY_WARNINGS}>
    )

    target_link_libraries(${target_name} PRIVATE hello_project_warnings)
endfunction()
