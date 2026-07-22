// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SingleInstance.hpp"
#include "mo/core/Constants.hpp"
#include "mo/core/Logger.hpp"

#include <QDBusConnection>
#include <QDBusConnectionInterface>

namespace mo::app {

SingleInstance::SingleInstance(QObject *parent)
    : QObject(parent)
    , sharedMemory_(mo::core::constants::kSharedMemoryKey)
{
}

SingleInstance::~SingleInstance() = default;

bool SingleInstance::tryLock()
{
    // Primary mechanism: register a well-known D-Bus service on the session bus.
    // On Wayland/Linux this both detects a previous instance and provides an
    // activation path for future instances.
    auto bus = QDBusConnection::sessionBus();
    if (bus.isConnected()) {
        if (bus.registerService(mo::core::constants::kDBusServiceName)) {
            locked_ = true;
            mo::core::Logger::info("Single instance acquired via D-Bus");
            return true;
        }
        mo::core::Logger::warning(
            "D-Bus service already registered; another instance is running");
        return false;
    }

    // Fallback mechanism: a shared memory segment.
    if (!sharedMemory_.create(1)) {
        if (sharedMemory_.attach()) {
            sharedMemory_.detach();
        }
        mo::core::Logger::warning(
            "Another instance is already running (QSharedMemory fallback)");
        return false;
    }

    locked_ = true;
    return true;
}

} // namespace mo::app
