# SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

# Expose the .qrc file path; AUTORCC (enabled via qt_standard_project_setup)
# will process it automatically when added to the target's sources.
set(MO_RESOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc
    PARENT_SCOPE
)
