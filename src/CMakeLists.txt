# SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
# SPDX-License-Identifier: GPL-3.0-or-later

add_subdirectory(core)
add_subdirectory(models)
add_subdirectory(services)
add_subdirectory(ui)
add_subdirectory(app)
add_subdirectory(resources)

qt_add_executable(hello MANUAL_FINALIZATION
    main.cpp
    ${HELLO_RESOURCE_FILES}
)

target_link_libraries(hello PRIVATE
    Hello::App
    Hello::Ui
    Hello::Services
    Hello::Models
    Hello::Core
    Qt6::Widgets
)

set_target_properties(hello PROPERTIES
    WIN32_EXECUTABLE OFF
    MACOSX_BUNDLE OFF
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

qt_finalize_executable(hello)
