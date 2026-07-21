# SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_library(hello_core STATIC)

add_library(Hello::Core ALIAS hello_core)

target_sources(hello_core
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/hello/core/Constants.hpp
            include/hello/core/Settings.hpp
            include/hello/core/Logger.hpp
)

target_sources(hello_core
    PRIVATE
        src/Settings.cpp
        src/Logger.cpp
)

target_compile_features(hello_core PUBLIC cxx_std_20)

target_link_libraries(hello_core
    PUBLIC
        Qt6::Core
)
