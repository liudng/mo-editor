// SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <memory>

class QSettings;

namespace hello::core {

class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString fontFamily READ fontFamily WRITE setFontFamily NOTIFY fontFamilyChanged)
    Q_PROPERTY(int fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged)
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(int tabWidth READ tabWidth WRITE setTabWidth NOTIFY tabWidthChanged)
    Q_PROPERTY(bool showLineNumbers READ showLineNumbers WRITE setShowLineNumbers NOTIFY showLineNumbersChanged)
    Q_PROPERTY(bool autoIndent READ autoIndent WRITE setAutoIndent NOTIFY autoIndentChanged)
    Q_PROPERTY(QStringList recentFiles READ recentFiles WRITE setRecentFiles NOTIFY recentFilesChanged)

public:
    enum class Theme { System, Light, Dark };
    Q_ENUM(Theme)

    static Settings &instance();

    void load();
    void save();

    QString fontFamily() const;
    void setFontFamily(const QString &font);

    int fontSize() const;
    void setFontSize(int size);

    QString theme() const;
    void setTheme(const QString &theme);

    int tabWidth() const;
    void setTabWidth(int width);

    bool showLineNumbers() const;
    void setShowLineNumbers(bool show);

    bool autoIndent() const;
    void setAutoIndent(bool enabled);

    QStringList recentFiles() const;
    void setRecentFiles(const QStringList &files);

    void addRecentFile(const QString &path);

signals:
    void fontFamilyChanged(const QString &font);
    void fontSizeChanged(int size);
    void themeChanged(const QString &theme);
    void tabWidthChanged(int width);
    void showLineNumbersChanged(bool show);
    void autoIndentChanged(bool enabled);
    void recentFilesChanged(const QStringList &files);

private:
    Settings();
    ~Settings() override;
    Settings(const Settings &) = delete;
    Settings &operator=(const Settings &) = delete;

    std::unique_ptr<QSettings> settings_;

    QString fontFamily_ = QStringLiteral("Monospace");
    int fontSize_ = 11;
    QString theme_ = QStringLiteral("System");
    int tabWidth_ = 4;
    bool showLineNumbers_ = true;
    bool autoIndent_ = true;
    QStringList recentFiles_;
};

} // namespace hello::core
