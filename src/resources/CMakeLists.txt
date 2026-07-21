# SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
# SPDX-License-Identifier: GPL-3.0-or-later

# Expose the .qrc file path; AUTORCC (enabled via qt_standard_project_setup)
# will process it automatically when added to the target's sources.
set(HELLO_RESOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc
    PARENT_SCOPE
)
