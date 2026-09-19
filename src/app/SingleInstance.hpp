// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QSharedMemory>
#include <QStringList>

namespace mo::app {

class SingleInstance : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "io.github.liudng.mo")

public:
    explicit SingleInstance(QObject *parent = nullptr);
    ~SingleInstance() override;

    // Returns true if this is the first (owning) instance.
    bool tryLock();

    // Asks the already-running instance to open the given files. Returns
    // true if the request could be delivered; only meaningful after
    // tryLock() has failed.
    bool forwardToRunningInstance(const QStringList &files) const;

signals:
    // Emitted on the running instance when another process asks it to
    // open files (see forwardToRunningInstance()).
    void openFilesRequested(const QStringList &files);

public slots:
    // D-Bus method "openFiles"; invoked by other processes of this app.
    void openFiles(const QStringList &files);

private:
    QSharedMemory sharedMemory_;
    bool locked_ = false;
};

} // namespace mo::app
