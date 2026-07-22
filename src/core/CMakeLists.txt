# SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_library(mo_core STATIC)

add_library(Mo::Core ALIAS mo_core)

target_sources(mo_core
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/mo/core/Constants.hpp
            include/mo/core/Settings.hpp
            include/mo/core/Logger.hpp
)

target_sources(mo_core
    PRIVATE
        src/Settings.cpp
        src/Logger.cpp
)

target_compile_features(mo_core PUBLIC cxx_std_20)

target_link_libraries(mo_core
    PUBLIC
        Qt6::Core
)
