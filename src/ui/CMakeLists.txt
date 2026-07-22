# SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_library(mo_ui STATIC)

add_library(Mo::Ui ALIAS mo_ui)

target_sources(mo_ui
    PRIVATE
        mainwindow/MainWindow.hpp
        mainwindow/MainWindow.cpp
        mainwindow/MainWindow.ui
        editor/CodeEditor.hpp
        editor/CodeEditor.cpp
        dialogs/FindReplaceDock.hpp
        dialogs/FindReplaceDock.cpp
        dialogs/SettingsDialog.hpp
        dialogs/SettingsDialog.cpp
)

target_include_directories(mo_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(mo_ui PUBLIC cxx_std_20)

target_link_libraries(mo_ui
    PUBLIC
        Qt6::Widgets
        Mo::Core
        Mo::Models
        Mo::Services
    PRIVATE
        KF6::SyntaxHighlighting
)
