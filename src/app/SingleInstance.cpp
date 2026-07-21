// SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SingleInstance.hpp"
#include "hello/core/Constants.hpp"
#include "hello/core/Logger.hpp"

#include <QDBusConnection>
#include <QDBusConnectionInterface>

namespace hello::app {

SingleInstance::SingleInstance(QObject *parent)
    : QObject(parent)
    , sharedMemory_(hello::core::constants::kSharedMemoryKey)
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
        if (bus.registerService(hello::core::constants::kDBusServiceName)) {
            locked_ = true;
            hello::core::Logger::info("Single instance acquired via D-Bus");
            return true;
        }
        hello::core::Logger::warning(
            "D-Bus service already registered; another instance is running");
        return false;
    }

    // Fallback mechanism: a shared memory segment.
    if (!sharedMemory_.create(1)) {
        if (sharedMemory_.attach()) {
            sharedMemory_.detach();
        }
        hello::core::Logger::warning(
            "Another instance is already running (QSharedMemory fallback)");
        return false;
    }

    locked_ = true;
    return true;
}

} // namespace hello::app
