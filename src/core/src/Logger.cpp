// SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "hello/core/Logger.hpp"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(helloLog, "hello")

namespace hello::core {

void Logger::init()
{
    // Default to stderr; Qt also forwards to journald on Linux when built
    // with journald support and stderr is unavailable.
    QLoggingCategory::setFilterRules(QStringLiteral(
        "hello.debug=true\n"
        "hello.info=true\n"
        "hello.warning=true\n"
        "hello.critical=true"));
}

void Logger::debug(const QString &message)
{
    qCDebug(helloLog).noquote() << message;
}

void Logger::info(const QString &message)
{
    qCInfo(helloLog).noquote() << message;
}

void Logger::warning(const QString &message)
{
    qCWarning(helloLog).noquote() << message;
}

void Logger::critical(const QString &message)
{
    qCCritical(helloLog).noquote() << message;
}

void Logger::error(const QString &message)
{
    qCCritical(helloLog).noquote() << message;
}

} // namespace hello::core
