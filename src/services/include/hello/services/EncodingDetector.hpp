// SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QByteArray>
#include <QString>

namespace hello::services {

class EncodingDetector
{
public:
    // Returns a charset name such as "UTF-8", "GB18030", "Big5".
    // Falls back to "UTF-8" when detection fails or input is empty.
    static QString detect(const QByteArray &data);
};

} // namespace hello::services
