// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QString>

namespace mo::services {

class FileService
{
public:
    // Reads a text file with the given encoding name; returns empty on failure.
    static QString readTextFile(const QString &path, const QString &encoding);

    // Atomically writes content to path using QSaveFile; returns true on success.
    static bool writeTextFile(const QString &path, const QString &content, const QString &encoding);
};

} // namespace mo::services
