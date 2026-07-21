# SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_library(hello_services STATIC)

add_library(Hello::Services ALIAS hello_services)

target_sources(hello_services
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/hello/services/EncodingDetector.hpp
            include/hello/services/FileService.hpp
            include/hello/services/SessionManager.hpp
)

target_sources(hello_services
    PRIVATE
        src/EncodingDetector.cpp
        src/FileService.cpp
        src/SessionManager.cpp
)

target_compile_features(hello_services PUBLIC cxx_std_20)

target_link_libraries(hello_services
    PUBLIC
        Qt6::Core
        Hello::Core
        Hello::Models
    PRIVATE
        uchardet::libuchardet
)
