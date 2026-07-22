# SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_library(mo_services STATIC)

add_library(Mo::Services ALIAS mo_services)

target_sources(mo_services
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/mo/services/EncodingDetector.hpp
            include/mo/services/FileService.hpp
            include/mo/services/SessionManager.hpp
)

target_sources(mo_services
    PRIVATE
        src/EncodingDetector.cpp
        src/FileService.cpp
        src/SessionManager.cpp
)

target_compile_features(mo_services PUBLIC cxx_std_20)

target_link_libraries(mo_services
    PUBLIC
        Qt6::Core
        Mo::Core
        Mo::Models
    PRIVATE
        uchardet::libuchardet
)
