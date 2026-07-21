# SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
# SPDX-License-Identifier: GPL-3.0-or-later

find_package(Qt6 REQUIRED COMPONENTS Test)

qt_add_executable(test_mainwindow test_mainwindow.cpp)
target_compile_features(test_mainwindow PRIVATE cxx_std_20)
target_link_libraries(test_mainwindow PRIVATE
    Qt6::Test
    Hello::Ui
)

# Run on the offscreen QPA platform so the suite works in headless CI.
set_target_properties(test_mainwindow PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

add_test(NAME test_mainwindow COMMAND test_mainwindow)
