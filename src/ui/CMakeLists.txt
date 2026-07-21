# SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_library(hello_ui STATIC)

add_library(Hello::Ui ALIAS hello_ui)

target_sources(hello_ui
    PRIVATE
        mainwindow/MainWindow.hpp
        mainwindow/MainWindow.cpp
        mainwindow/MainWindow.ui
        editor/CodeEditor.hpp
        editor/CodeEditor.cpp
        dialogs/FindReplaceDialog.hpp
        dialogs/FindReplaceDialog.cpp
        dialogs/SettingsDialog.hpp
        dialogs/SettingsDialog.cpp
)

target_include_directories(hello_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(hello_ui PUBLIC cxx_std_20)

target_link_libraries(hello_ui
    PUBLIC
        Qt6::Widgets
        Hello::Core
        Hello::Models
        Hello::Services
    PRIVATE
        KF6::SyntaxHighlighting
)
