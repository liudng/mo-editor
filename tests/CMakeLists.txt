# SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

find_package(Qt6 REQUIRED COMPONENTS Test)

enable_testing()

if(MO_BUILD_TESTS)
    add_subdirectory(unit)
endif()

add_subdirectory(integration)

if(MO_BUILD_FUZZ)
    add_subdirectory(fuzz)
endif()
