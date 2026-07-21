# SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
# SPDX-License-Identifier: GPL-3.0-or-later

# Applies reproducible-build options to the given target. Strips build
# host paths from debug info and honors SOURCE_DATE_EPOCH for the
# __DATE__ / __TIME__ macros.
function(apply_reproducible_builds target_name)
    add_library(hello_reproducible_builds INTERFACE)

    target_compile_options(hello_reproducible_builds INTERFACE
        -ffile-prefix-map=${CMAKE_SOURCE_DIR}=.
        -ffile-prefix-map=${CMAKE_BINARY_DIR}=.
        -fdebug-prefix-map=${CMAKE_SOURCE_DIR}=.
    )

    # When SOURCE_DATE_EPOCH is present, feed it to the compiler so that
    # __DATE__/__TIME__ expand deterministically. -Wdate-time surfaces any
    # remaining use of those macros.
    if(DEFINED ENV{SOURCE_DATE_EPOCH})
        target_compile_definitions(hello_reproducible_builds INTERFACE
            SOURCE_DATE_EPOCH=$ENV{SOURCE_DATE_EPOCH}
        )
        target_compile_options(hello_reproducible_builds INTERFACE
            -Wdate-time
        )
    endif()

    # RPATH handling: keep RPATH for dev builds but avoid embedding
    # absolute build-tree paths into the installed artifact.
    set(CMAKE_SKIP_RPATH OFF PARENT_SCOPE)
    set(CMAKE_BUILD_RPATH_USE_ORIGIN ON PARENT_SCOPE)

    target_link_libraries(${target_name} PRIVATE hello_reproducible_builds)
endfunction()
