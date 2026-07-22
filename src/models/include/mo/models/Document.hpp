// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QString>

namespace mo::models {

class Document : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath NOTIFY filePathChanged)
    Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged)
    Q_PROPERTY(bool isModified READ isModified NOTIFY modifiedChanged)
    Q_PROPERTY(QString encoding READ encoding NOTIFY encodingChanged)
    Q_PROPERTY(QString language READ language NOTIFY languageChanged)

public:
    explicit Document(QObject *parent = nullptr);

    QString filePath() const;
    QString content() const;
    bool isModified() const;
    QString encoding() const;
    QString language() const;

    void setContent(const QString &content);
    void setEncoding(const QString &encoding);
    void setLanguage(const QString &language);

    bool load(const QString &path);
    bool save();
    bool saveAs(const QString &path);

signals:
    void filePathChanged(const QString &path);
    void contentChanged(const QString &content);
    void modifiedChanged(bool modified);
    void encodingChanged(const QString &encoding);
    void languageChanged(const QString &language);

private:
    void setModified(bool modified);
    void setFilePath(const QString &path);

    QString filePath_;
    QString content_;
    bool modified_ = false;
    QString encoding_ = QStringLiteral("UTF-8");
    QString language_ = QStringLiteral("Plain Text");
};

} // namespace mo::models
