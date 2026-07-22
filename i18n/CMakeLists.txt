# SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

# The mo target is created by src/CMakeLists.txt which runs before this
# directory (see top-level CMakeLists.txt add_subdirectory ordering).

set(mo_TS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/mo_zh_CN.ts
)

# qt_add_translations generates .qm from .ts AND embeds them into the mo
# target's resources at :/i18n/<basename>.qm, which matches the load path in
# Application::loadTranslation(). The .qm files are baked into the executable
# so no separate runtime installation is required.
qt_add_translations(mo
    TS_FILES ${mo_TS_FILES}
    RESOURCE_PREFIX "/i18n"
)
