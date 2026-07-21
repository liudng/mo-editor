// SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "hello/models/Document.hpp"

#include <QFile>
#include <QFileInfo>
#include <QStringConverter>
#include <QTextStream>

namespace hello::models {

Document::Document(QObject *parent) : QObject(parent) {}

QString Document::filePath() const { return filePath_; }
QString Document::content() const { return content_; }
bool Document::isModified() const { return modified_; }
QString Document::encoding() const { return encoding_; }
QString Document::language() const { return language_; }

void Document::setContent(const QString &content)
{
    if (content_ != content) {
        content_ = content;
        setModified(true);
        emit contentChanged(content_);
    }
}

void Document::setEncoding(const QString &encoding)
{
    if (encoding_ != encoding) {
        encoding_ = encoding;
        emit encodingChanged(encoding_);
    }
}

void Document::setLanguage(const QString &language)
{
    if (language_ != language) {
        language_ = language;
        emit languageChanged(language_);
    }
}

bool Document::load(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    content_ = in.readAll();
    file.close();

    setFilePath(path);
    setModified(false);
    emit contentChanged(content_);
    return true;
}

bool Document::save()
{
    if (filePath_.isEmpty()) {
        return false;
    }
    return saveAs(filePath_);
}

bool Document::saveAs(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << content_;
    file.close();

    setFilePath(path);
    setModified(false);
    return true;
}

void Document::setModified(bool modified)
{
    if (modified_ != modified) {
        modified_ = modified;
        emit modifiedChanged(modified_);
    }
}

void Document::setFilePath(const QString &path)
{
    if (filePath_ != path) {
        filePath_ = path;
        emit filePathChanged(path);
    }
}

} // namespace hello::models
