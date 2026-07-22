# SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_library(mo_models STATIC)

add_library(Mo::Models ALIAS mo_models)

target_sources(mo_models
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/mo/models/Document.hpp
)

target_sources(mo_models
    PRIVATE
        src/Document.cpp
)

target_compile_features(mo_models PUBLIC cxx_std_20)

target_link_libraries(mo_models
    PUBLIC
        Qt6::Core
        Mo::Core
)
