// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QStringList>
#include <optional>

namespace mo::services {

struct SessionState
{
    QStringList openFiles;
    QStringList cursorPositions;
    QByteArray windowGeometry;
    QByteArray windowState;
};

class SessionManager : public QObject
{
    Q_OBJECT

public:
    explicit SessionManager(QObject *parent = nullptr);

    void saveSession(const SessionState &state);
    std::optional<SessionState> loadSession() const;
    void clearSession();
};

} // namespace mo::services
