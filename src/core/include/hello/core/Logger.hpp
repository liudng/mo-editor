// SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QString>

namespace hello::core {

class Logger
{
public:
    static void init();
    static void debug(const QString &message);
    static void info(const QString &message);
    static void warning(const QString &message);
    static void critical(const QString &message);
    static void error(const QString &message);
};

} // namespace hello::core
