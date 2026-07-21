// SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QSharedMemory>

namespace hello::app {

class SingleInstance : public QObject
{
    Q_OBJECT

public:
    explicit SingleInstance(QObject *parent = nullptr);
    ~SingleInstance() override;

    // Returns true if this is the first (owning) instance.
    bool tryLock();

signals:
    // Emitted on the running instance when another process attempts to start.
    void anotherInstanceStarted();

private:
    QSharedMemory sharedMemory_;
    bool locked_ = false;
};

} // namespace hello::app
