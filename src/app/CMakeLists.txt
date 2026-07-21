# SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_library(hello_app STATIC
    Application.hpp
    Application.cpp
    SingleInstance.hpp
    SingleInstance.cpp
)

add_library(Hello::App ALIAS hello_app)

target_compile_features(hello_app PUBLIC cxx_std_20)

target_link_libraries(hello_app
    PUBLIC
        Qt6::Widgets
    PRIVATE
        Qt6::DBus
        Hello::Core
        Hello::Ui
)
