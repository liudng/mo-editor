# SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_executable(test_settings test_settings.cpp)
target_compile_features(test_settings PRIVATE cxx_std_20)
target_link_libraries(test_settings PRIVATE
    Qt6::Test
    Mo::Core
)
# Explicitly link the C++ standard library. When linking a static library
# (Mo::Core) into a test executable, the linker may not pull in libstdc++
# automatically, causing "DSO missing from command line" errors for
# operator new/delete symbols.
target_link_options(test_settings PRIVATE -lstdc++)
add_test(NAME test_settings COMMAND test_settings)

qt_add_executable(test_document test_document.cpp)
target_compile_features(test_document PRIVATE cxx_std_20)
target_link_libraries(test_document PRIVATE
    Qt6::Test
    Mo::Models
)
target_link_options(test_document PRIVATE -lstdc++)
add_test(NAME test_document COMMAND test_document)

qt_add_executable(test_encoding_detector test_encoding_detector.cpp)
target_compile_features(test_encoding_detector PRIVATE cxx_std_20)
target_link_libraries(test_encoding_detector PRIVATE
    Qt6::Test
    Mo::Services
)
target_link_options(test_encoding_detector PRIVATE -lstdc++)
add_test(NAME test_encoding_detector COMMAND test_encoding_detector)
