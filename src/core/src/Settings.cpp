// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "mo/core/Settings.hpp"
#include "mo/core/Constants.hpp"

#include <QDir>
#include <QSettings>
#include <QStandardPaths>

namespace mo::core {

Settings &Settings::instance()
{
    static Settings s;
    return s;
}

Settings::Settings()
{
    const auto configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configDir);
    const auto path = configDir + QDir::separator() + mo::core::constants::kSettingsFile;
    settings_ = std::make_unique<QSettings>(path, QSettings::IniFormat);
    // Qt6 reads/writes INI files as UTF-8 by default; no codec setup needed.
}

Settings::~Settings() = default;

void Settings::load()
{
    settings_->sync();
    setFontFamily(settings_->value(QStringLiteral("ui/fontFamily"), fontFamily_).toString());
    setFontSize(settings_->value(QStringLiteral("ui/fontSize"), fontSize_).toInt());
    setTheme(settings_->value(QStringLiteral("ui/theme"), theme_).toString());
    setIconTheme(settings_->value(QStringLiteral("ui/iconTheme"), iconTheme_).toString());
    setTabWidth(settings_->value(QStringLiteral("editor/tabWidth"), tabWidth_).toInt());
    setShowLineNumbers(settings_->value(QStringLiteral("editor/showLineNumbers"), showLineNumbers_).toBool());
    setAutoIndent(settings_->value(QStringLiteral("editor/autoIndent"), autoIndent_).toBool());
    setRecentFiles(settings_->value(QStringLiteral("recent/files"), recentFiles_).toStringList());
}

void Settings::save()
{
    settings_->setValue(QStringLiteral("ui/fontFamily"), fontFamily_);
    settings_->setValue(QStringLiteral("ui/fontSize"), fontSize_);
    settings_->setValue(QStringLiteral("ui/theme"), theme_);
    settings_->setValue(QStringLiteral("ui/iconTheme"), iconTheme_);
    settings_->setValue(QStringLiteral("editor/tabWidth"), tabWidth_);
    settings_->setValue(QStringLiteral("editor/showLineNumbers"), showLineNumbers_);
    settings_->setValue(QStringLiteral("editor/autoIndent"), autoIndent_);
    settings_->setValue(QStringLiteral("recent/files"), recentFiles_);
    settings_->sync();
}

QString Settings::fontFamily() const { return fontFamily_; }

void Settings::setFontFamily(const QString &font)
{
    if (fontFamily_ != font) {
        fontFamily_ = font;
        emit fontFamilyChanged(fontFamily_);
    }
}

int Settings::fontSize() const { return fontSize_; }

void Settings::setFontSize(int size)
{
    if (fontSize_ != size) {
        fontSize_ = size;
        emit fontSizeChanged(fontSize_);
    }
}

QString Settings::theme() const { return theme_; }

void Settings::setTheme(const QString &theme)
{
    if (theme_ != theme) {
        theme_ = theme;
        emit themeChanged(theme_);
    }
}

QString Settings::iconTheme() const { return iconTheme_; }

void Settings::setIconTheme(const QString &iconTheme)
{
    if (iconTheme_ != iconTheme) {
        iconTheme_ = iconTheme;
        emit iconThemeChanged(iconTheme_);
    }
}

int Settings::tabWidth() const { return tabWidth_; }

void Settings::setTabWidth(int width)
{
    if (tabWidth_ != width) {
        tabWidth_ = width;
        emit tabWidthChanged(tabWidth_);
    }
}

bool Settings::showLineNumbers() const { return showLineNumbers_; }

void Settings::setShowLineNumbers(bool show)
{
    if (showLineNumbers_ != show) {
        showLineNumbers_ = show;
        emit showLineNumbersChanged(showLineNumbers_);
    }
}

bool Settings::autoIndent() const { return autoIndent_; }

void Settings::setAutoIndent(bool enabled)
{
    if (autoIndent_ != enabled) {
        autoIndent_ = enabled;
        emit autoIndentChanged(autoIndent_);
    }
}

QStringList Settings::recentFiles() const { return recentFiles_; }

void Settings::setRecentFiles(const QStringList &files)
{
    if (recentFiles_ != files) {
        recentFiles_ = files;
        emit recentFilesChanged(recentFiles_);
    }
}

void Settings::addRecentFile(const QString &path)
{
    recentFiles_.removeAll(path);
    recentFiles_.prepend(path);
    while (recentFiles_.size() > 10) {
        recentFiles_.removeLast();
    }
    emit recentFilesChanged(recentFiles_);
}

} // namespace mo::core
