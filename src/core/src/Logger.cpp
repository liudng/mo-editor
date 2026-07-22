// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "mo/core/Logger.hpp"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(moLog, "mo")

namespace mo::core {

void Logger::init()
{
    // Default to stderr; Qt also forwards to journald on Linux when built
    // with journald support and stderr is unavailable.
    QLoggingCategory::setFilterRules(QStringLiteral(
        "mo.debug=true\n"
        "mo.info=true\n"
        "mo.warning=true\n"
        "mo.critical=true"));
}

void Logger::debug(const QString &message)
{
    qCDebug(moLog).noquote() << message;
}

void Logger::info(const QString &message)
{
    qCInfo(moLog).noquote() << message;
}

void Logger::warning(const QString &message)
{
    qCWarning(moLog).noquote() << message;
}

void Logger::critical(const QString &message)
{
    qCCritical(moLog).noquote() << message;
}

void Logger::error(const QString &message)
{
    qCCritical(moLog).noquote() << message;
}

} // namespace mo::core
