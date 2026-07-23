# SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.31)

project(mo
    VERSION 0.1.0
    DESCRIPTION "A Qt6 text editor"
    HOMEPAGE_URL "https://github.com/liudng/mo"
    LANGUAGES CXX
)

# ---------------------------------------------------------------------------
# C++ standard
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------------------------
# Globally link the C++ standard library.
# ---------------------------------------------------------------------------
# On some toolchains (notably GCC/Clang on Ubuntu 26.04 with gold/bfd ld),
# linking a static library into an executable does not automatically pull in
# libstdc++, causing "DSO missing from command line" errors for operator
# new/delete and std::terminate. link_libraries() applies to all targets
# created after this point, ensuring libstdc++ appears in the correct link
# position (after object files).
link_libraries(stdc++)

# ---------------------------------------------------------------------------
# Build options
# ---------------------------------------------------------------------------
option(MO_BUILD_TESTS "Build the unit and integration tests" ON)
option(MO_BUILD_FUZZ "Build the fuzz targets (requires a fuzzer-capable toolchain)" OFF)

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------
find_package(Qt6 6.8 REQUIRED COMPONENTS Widgets LinguistTools Svg Concurrent)
find_package(KF6SyntaxHighlighting REQUIRED)
find_package(uchardet REQUIRED)

qt_standard_project_setup(
    REQUIRES 6.8
    I18N_SOURCE_LANGUAGE en
    I18N_TRANSLATED_LANGUAGES zh_CN
)

# ---------------------------------------------------------------------------
# Qt project policies
# ---------------------------------------------------------------------------
foreach(policy QTP0001 QTP0002 QTP0003 QTP0004 QTP0005)
    if(POLICY ${policy})
        qt_policy(SET ${policy} NEW)
    endif()
endforeach()

# ---------------------------------------------------------------------------
# Custom CMake modules
# ---------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(CompilerWarnings OPTIONAL)
include(Hardening OPTIONAL)
include(Sanitizers OPTIONAL)
include(ReproducibleBuilds OPTIONAL)
include(StaticAnalyzers OPTIONAL)
include(Coverage OPTIONAL)

# ---------------------------------------------------------------------------
# Generated version header
# ---------------------------------------------------------------------------
if(EXISTS "${CMAKE_SOURCE_DIR}/src/core/include/mo/core/Version.hpp.in")
    configure_file(
        "${CMAKE_SOURCE_DIR}/src/core/include/mo/core/Version.hpp.in"
        "${CMAKE_BINARY_DIR}/include/mo/core/Version.hpp"
        @ONLY
    )
endif()

# ---------------------------------------------------------------------------
# Testing
# ---------------------------------------------------------------------------
enable_testing()
include(CTest)

# ---------------------------------------------------------------------------
# Subdirectories
# ---------------------------------------------------------------------------
if(EXISTS "${CMAKE_SOURCE_DIR}/src/CMakeLists.txt")
    add_subdirectory(src)
endif()

# i18n must come after src/ so the mo target exists for qt_add_translations.
if(EXISTS "${CMAKE_SOURCE_DIR}/i18n/CMakeLists.txt")
    add_subdirectory(i18n)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/packaging/install/CMakeLists.txt")
    add_subdirectory(packaging/install)
endif()

# ---------------------------------------------------------------------------
# Packaging & SBOM
# ---------------------------------------------------------------------------
include(Packaging OPTIONAL)
include(Sbom OPTIONAL)
