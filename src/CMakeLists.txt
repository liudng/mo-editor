# SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

add_subdirectory(core)
add_subdirectory(models)
add_subdirectory(services)
add_subdirectory(ui)
add_subdirectory(app)
add_subdirectory(resources)

qt_add_executable(mo MANUAL_FINALIZATION
    main.cpp
    ${MO_RESOURCE_FILES}
)

target_link_libraries(mo PRIVATE
    Mo::App
    Mo::Ui
    Mo::Services
    Mo::Models
    Mo::Core
    Qt6::Widgets
)

set_target_properties(mo PROPERTIES
    WIN32_EXECUTABLE OFF
    MACOSX_BUNDLE OFF
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

qt_finalize_executable(mo)
