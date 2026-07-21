# SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_library(hello_models STATIC)

add_library(Hello::Models ALIAS hello_models)

target_sources(hello_models
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/hello/models/Document.hpp
)

target_sources(hello_models
    PRIVATE
        src/Document.cpp
)

target_compile_features(hello_models PUBLIC cxx_std_20)

target_link_libraries(hello_models
    PUBLIC
        Qt6::Core
        Hello::Core
)
