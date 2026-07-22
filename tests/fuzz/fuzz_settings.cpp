// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <QByteArray>
#include <QString>

#include "mo/core/Settings.hpp"

// Stub entry point used by libFuzzer. When libFuzzer is not linked, this binary
// can still be built for smoke-testing by providing a main() in a separate unit.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size == 0 || size > 64 * 1024) {
        return 0;
    }

    const QByteArray input(reinterpret_cast<const char*>(data), static_cast<int>(size));

    // Treat the fuzz input as a candidate path for the recent-files list and as
    // a candidate settings payload. The goal is to exercise parsing without
    // crashing or leaking.
    auto &settings = mo::core::Settings::instance();
    settings.addRecentFile(QString::fromUtf8(input));
    settings.load();

    return 0;
}

#ifndef MO_FUZZ_HAS_MAIN
// Provide a tiny main() so the target links cleanly when libFuzzer is absent,
// e.g. in regular CI builds that only want to compile-check the source.
int main(int argc, char** argv) {
    const uint8_t fallback[] = "fuzz";
    if (argc > 1) {
        const auto* data = reinterpret_cast<const uint8_t*>(argv[1]);
        return LLVMFuzzerTestOneInput(data, std::strlen(argv[1]));
    }
    return LLVMFuzzerTestOneInput(fallback, sizeof(fallback) - 1);
}
#endif
