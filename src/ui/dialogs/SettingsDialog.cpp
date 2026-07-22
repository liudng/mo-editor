// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SettingsDialog.hpp"

#include "mo/core/Settings.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QFontComboBox>
#include <QFormLayout>
#include <QIcon>
#include <QSet>
#include <QSpinBox>
#include <QStandardPaths>
#include <QVBoxLayout>

namespace mo::ui {

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Preferences"));

    fontCombo_ = new QFontComboBox(this);
    fontSizeSpin_ = new QSpinBox(this);
    fontSizeSpin_->setRange(6, 48);

    themeCombo_ = new QComboBox(this);
    themeCombo_->addItem(tr("System"), QStringLiteral("System"));
    themeCombo_->addItem(tr("Light"), QStringLiteral("Light"));
    themeCombo_->addItem(tr("Dark"), QStringLiteral("Dark"));

    iconThemeCombo_ = new QComboBox(this);
    iconThemeCombo_->setToolTip(tr("Fallback icon theme used when the system "
                                    "theme is missing icons (e.g. Adwaita)."));
    // Populate with icon themes found in standard search paths.
    iconThemeCombo_->addItem(tr("System Default"), QString());
    const auto iconPaths = QIcon::themeSearchPaths();
    QSet<QString> seen;
    for (const auto &path : iconPaths) {
        QDir dir(path);
        if (!dir.exists()) {
            continue;
        }
        QDirIterator it(path, QDir::Dirs | QDir::NoDotAndDotDot);
        while (it.hasNext()) {
            it.next();
            // Only list directories that look like icon themes (contain index.theme).
            if (QFileInfo::exists(it.filePath() + QStringLiteral("/index.theme"))) {
                const auto name = it.fileName();
                if (!seen.contains(name)) {
                    seen.insert(name);
                    iconThemeCombo_->addItem(name, name);
                }
            }
        }
    }

    tabWidthSpin_ = new QSpinBox(this);
    tabWidthSpin_->setRange(1, 16);

    lineNumbersCheck_ = new QCheckBox(tr("Show line numbers"), this);
    autoIndentCheck_ = new QCheckBox(tr("Auto indent"), this);

    auto *form = new QFormLayout;
    form->addRow(tr("Font:"), fontCombo_);
    form->addRow(tr("Font size:"), fontSizeSpin_);
    form->addRow(tr("Theme:"), themeCombo_);
    form->addRow(tr("Icon theme:"), iconThemeCombo_);
    form->addRow(tr("Tab width:"), tabWidthSpin_);
    form->addRow(lineNumbersCheck_);
    form->addRow(autoIndentCheck_);

    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &SettingsDialog::onAccept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);

    load();
}

void SettingsDialog::load()
{
    auto &s = mo::core::Settings::instance();
    fontCombo_->setCurrentFont(QFont(s.fontFamily()));
    fontSizeSpin_->setValue(s.fontSize());

    const auto theme = s.theme();
    for (int i = 0; i < themeCombo_->count(); ++i) {
        if (themeCombo_->itemData(i).toString() == theme) {
            themeCombo_->setCurrentIndex(i);
            break;
        }
    }

    const auto iconTheme = s.iconTheme();
    bool found = false;
    for (int i = 0; i < iconThemeCombo_->count(); ++i) {
        const auto data = iconThemeCombo_->itemData(i).toString();
        if (data == iconTheme || (data.isEmpty() && iconTheme.isEmpty())) {
            iconThemeCombo_->setCurrentIndex(i);
            found = true;
            break;
        }
    }
    if (!found) {
        // The configured icon theme is not installed; show it as a custom entry.
        iconThemeCombo_->insertItem(0, iconTheme, iconTheme);
        iconThemeCombo_->setCurrentIndex(0);
    }

    tabWidthSpin_->setValue(s.tabWidth());
    lineNumbersCheck_->setChecked(s.showLineNumbers());
    autoIndentCheck_->setChecked(s.autoIndent());
}

void SettingsDialog::save()
{
    auto &s = mo::core::Settings::instance();
    s.setFontFamily(fontCombo_->currentFont().family());
    s.setFontSize(fontSizeSpin_->value());
    s.setTheme(themeCombo_->currentData().toString());
    s.setIconTheme(iconThemeCombo_->currentData().toString());
    s.setTabWidth(tabWidthSpin_->value());
    s.setShowLineNumbers(lineNumbersCheck_->isChecked());
    s.setAutoIndent(autoIndentCheck_->isChecked());
    s.save();
}

void SettingsDialog::onAccept()
{
    save();
    emit settingsChanged();
    accept();
}

} // namespace mo::ui
