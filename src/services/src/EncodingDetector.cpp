// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "mo/services/EncodingDetector.hpp"

#include <uchardet/uchardet.h>

namespace mo::services {

QString EncodingDetector::detect(const QByteArray &data)
{
    if (data.isEmpty()) {
        return QStringLiteral("UTF-8");
    }

    uchardet_t ud = uchardet_new();
    uchardet_handle_data(ud, data.constData(), static_cast<size_t>(data.size()));
    uchardet_data_end(ud);

    const char *charset = uchardet_get_charset(ud);
    QString result = charset ? QString::fromLatin1(charset) : QString();

    uchardet_delete(ud);

    if (result.isEmpty()) {
        result = QStringLiteral("UTF-8");
    }
    return result;
}

} // namespace mo::services
