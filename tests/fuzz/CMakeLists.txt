# SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

# Fuzz targets are optional and require a sanitizer / fuzzer-capable toolchain.
# Enable with -DMO_BUILD_FUZZ=ON and a compiler that supports -fsanitize=fuzzer.

find_package(Qt6 REQUIRED COMPONENTS Core)

add_executable(fuzz_settings fuzz_settings.cpp)
target_compile_features(fuzz_settings PRIVATE cxx_std_20)
target_link_libraries(fuzz_settings PRIVATE
    Qt6::Core
    Mo::Core
)

option(MO_FUZZ_LINK_LIBFUZZER "Link fuzz_settings against libFuzzer" ON)
if(MO_FUZZ_LINK_LIBFUZZER)
    target_compile_options(fuzz_settings PRIVATE -fsanitize=fuzzer-no-link)
    target_link_options(fuzz_settings PRIVATE -fsanitize=fuzzer)
endif()
