# SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

include(GNUInstallDirs)

# ----------------------------------------------------------------------------
# Executable
# ----------------------------------------------------------------------------
install(TARGETS mo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ----------------------------------------------------------------------------
# Desktop integration
# ----------------------------------------------------------------------------
# Render the .desktop and D-Bus service templates with the real install prefix.
configure_file(
    ${CMAKE_SOURCE_DIR}/data/io.github.liudng.mo.desktop.in
    ${CMAKE_CURRENT_BINARY_DIR}/io.github.liudng.mo.desktop
    @ONLY
)
configure_file(
    ${CMAKE_SOURCE_DIR}/data/io.github.liudng.mo.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/io.github.liudng.mo.service
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/io.github.liudng.mo.desktop
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
)

install(FILES
    ${CMAKE_SOURCE_DIR}/data/io.github.liudng.mo.metainfo.xml
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/io.github.liudng.mo.service
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/dbus-1/services
)

install(FILES
    ${CMAKE_SOURCE_DIR}/data/mime/io.github.liudng.mo.xml
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/mime/packages
)

# ----------------------------------------------------------------------------
# Icons (hicolor)
# ----------------------------------------------------------------------------
install(FILES
    ${CMAKE_SOURCE_DIR}/data/icons/hicolor/scalable/apps/io.github.liudng.mo.svg
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps
)

# ----------------------------------------------------------------------------
# Translations
# ----------------------------------------------------------------------------
install(FILES
    ${CMAKE_BINARY_DIR}/i18n/mo_zh_CN.qm
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/mo/translations
    OPTIONAL
)

# ----------------------------------------------------------------------------
# Man page
# ----------------------------------------------------------------------------
install(FILES
    ${CMAKE_SOURCE_DIR}/docs/man/mo.1
    DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
)
