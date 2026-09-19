// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SingleInstance.hpp"
#include "mo/core/Constants.hpp"
#include "mo/core/Logger.hpp"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>

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
            // Expose the "openFiles" method so that later instances can hand
            // their command-line file arguments over to this instance.
            if (!bus.registerObject(mo::core::constants::kDBusObjectPath, this,
                                    QDBusConnection::ExportAllSlots)) {
                mo::core::Logger::warning("Failed to register D-Bus object");
            }
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

void SingleInstance::openFiles(const QStringList &files)
{
    emit openFilesRequested(files);
}

bool SingleInstance::forwardToRunningInstance(const QStringList &files) const
{
    auto bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) {
        mo::core::Logger::warning("Cannot forward files: no D-Bus session bus");
        return false;
    }

    auto message = QDBusMessage::createMethodCall(
        mo::core::constants::kDBusServiceName,
        mo::core::constants::kDBusObjectPath,
        mo::core::constants::kDBusServiceName,
        QStringLiteral("openFiles"));
    message.setArguments({QVariant::fromValue(files)});
    if (!bus.send(message)) {
        mo::core::Logger::warning("Failed to forward files to the running instance");
        return false;
    }
    mo::core::Logger::info("Forwarded files to the running instance");
    return true;
}

} // namespace mo::app
