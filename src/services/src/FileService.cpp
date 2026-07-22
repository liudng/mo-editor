// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "mo/services/FileService.hpp"
#include "mo/core/Logger.hpp"

#include <QFile>
#include <QSaveFile>
#include <QStringConverter>
#include <QTextStream>

namespace mo::services {

namespace {

QStringConverter::Encoding encodingFromName(const QString &name)
{
    const auto lower = name.toLower();
    if (lower == "utf-8" || lower == "utf8") {
        return QStringConverter::Utf8;
    }
    if (lower == "utf-16le") {
        return QStringConverter::Utf16LE;
    }
    if (lower == "utf-16be") {
        return QStringConverter::Utf16BE;
    }
    if (lower == "latin-1" || lower == "iso-8859-1") {
        return QStringConverter::Latin1;
    }
    return QStringConverter::Utf8;
}

} // namespace

QString FileService::readTextFile(const QString &path, const QString &encoding)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        mo::core::Logger::warning("Failed to open file: " + path);
        return {};
    }
    QTextStream in(&file);
    in.setEncoding(encodingFromName(encoding));
    return in.readAll();
}

bool FileService::writeTextFile(const QString &path, const QString &content, const QString &encoding)
{
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        mo::core::Logger::warning("Failed to open file for writing: " + path);
        return false;
    }
    QTextStream out(&file);
    out.setEncoding(encodingFromName(encoding));
    out << content;
    out.flush();
    return file.commit();
}

} // namespace mo::services
