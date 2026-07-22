# SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_library(mo_app STATIC
    Application.hpp
    Application.cpp
    SingleInstance.hpp
    SingleInstance.cpp
)

add_library(Mo::App ALIAS mo_app)

target_compile_features(mo_app PUBLIC cxx_std_20)

target_link_libraries(mo_app
    PUBLIC
        Qt6::Widgets
    PRIVATE
        Qt6::DBus
        Mo::Core
        Mo::Ui
)
