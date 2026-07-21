// SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "hello/services/SessionManager.hpp"
#include "hello/core/Constants.hpp"

#include <QDir>
#include <QSettings>
#include <QStandardPaths>

namespace hello::services {

namespace {

QString settingsPath()
{
    const auto configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    return configDir + QDir::separator() + hello::core::constants::kSettingsFile;
}

} // namespace

SessionManager::SessionManager(QObject *parent) : QObject(parent) {}

void SessionManager::saveSession(const SessionState &state)
{
    QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
    QSettings settings(settingsPath(), QSettings::IniFormat);

    settings.beginGroup(QStringLiteral("session"));
    settings.setValue(QStringLiteral("openFiles"), state.openFiles);
    settings.setValue(QStringLiteral("cursorPositions"), state.cursorPositions);
    settings.setValue(QStringLiteral("windowGeometry"), state.windowGeometry);
    settings.setValue(QStringLiteral("windowState"), state.windowState);
    settings.endGroup();
    settings.sync();
}

std::optional<SessionState> SessionManager::loadSession() const
{
    QSettings settings(settingsPath(), QSettings::IniFormat);

    settings.beginGroup(QStringLiteral("session"));
    if (!settings.contains(QStringLiteral("openFiles"))) {
        settings.endGroup();
        return std::nullopt;
    }
    SessionState state;
    state.openFiles = settings.value(QStringLiteral("openFiles")).toStringList();
    state.cursorPositions = settings.value(QStringLiteral("cursorPositions")).toStringList();
    state.windowGeometry = settings.value(QStringLiteral("windowGeometry")).toByteArray();
    state.windowState = settings.value(QStringLiteral("windowState")).toByteArray();
    settings.endGroup();
    return state;
}

void SessionManager::clearSession()
{
    QSettings settings(settingsPath(), QSettings::IniFormat);
    settings.beginGroup(QStringLiteral("session"));
    settings.remove(QString());
    settings.endGroup();
    settings.sync();
}

} // namespace hello::services
