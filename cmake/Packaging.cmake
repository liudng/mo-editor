# SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

# CPack configuration: produces TGZ / DEB / RPM packages with Runtime
# and Development components. Include this module after the install()
# rules have been defined.

set(CPACK_PACKAGE_NAME "mo")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "liudng")
set(CPACK_PACKAGE_CONTACT "Liu Dong <liudng@hotmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Qt6 text editor")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/liudng/mo-editor")

# Use a single archive per component.
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

# Components.
set(CPACK_COMPONENTS_ALL Runtime Development)
set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "mo runtime")
set(CPACK_COMPONENT_RUNTIME_DESCRIPTION "mo text editor binary")
set(CPACK_COMPONENT_DEVELOPMENT_DISPLAY_NAME "mo development files")
set(CPACK_COMPONENT_DEVELOPMENT_DESCRIPTION "Headers and CMake config for mo")

# Build all enabled generators in one cpack invocation.
set(CPACK_GENERATOR "TGZ;DEB;RPM")

# --------------------------------------------------------------------------
# DEB
# --------------------------------------------------------------------------
set(CPACK_DEBIAN_PACKAGE_DEPENDS
    "qt6-base-dev, libkf6syntaxhighlighting-dev, libuchardet-dev")
set(CPACK_DEBIAN_PACKAGE_SECTION "editors")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Liu Dong <liudng@hotmail.com>")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/liudng/mo-editor")
set(CPACK_DEBIAN_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_RUNTIME_PACKAGE_NAME "mo")
set(CPACK_DEBIAN_DEVELOPMENT_PACKAGE_NAME "mo-dev")

# --------------------------------------------------------------------------
# RPM
# --------------------------------------------------------------------------
set(CPACK_RPM_PACKAGE_LICENSE "GPL-3.0-or-later")
set(CPACK_RPM_PACKAGE_REQUIRES
    "qt6-base-dev, libkf6syntaxhighlighting-dev, libuchardet-dev")
set(CPACK_RPM_PACKAGE_GROUP "Applications/Editors")
set(CPACK_RPM_PACKAGE_URL "https://github.com/liudng/mo-editor")
set(CPACK_RPM_COMPONENT_INSTALL ON)
set(CPACK_RPM_RUNTIME_PACKAGE_NAME "mo")
set(CPACK_RPM_DEVELOPMENT_PACKAGE_NAME "mo-dev")

include(CPack)
