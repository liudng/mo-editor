// SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SettingsDialog.hpp"

#include "hello/core/Settings.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFontComboBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QVBoxLayout>

namespace hello::ui {

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

    tabWidthSpin_ = new QSpinBox(this);
    tabWidthSpin_->setRange(1, 16);

    lineNumbersCheck_ = new QCheckBox(tr("Show line numbers"), this);
    autoIndentCheck_ = new QCheckBox(tr("Auto indent"), this);

    auto *form = new QFormLayout;
    form->addRow(tr("Font:"), fontCombo_);
    form->addRow(tr("Font size:"), fontSizeSpin_);
    form->addRow(tr("Theme:"), themeCombo_);
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
    auto &s = hello::core::Settings::instance();
    fontCombo_->setCurrentFont(QFont(s.fontFamily()));
    fontSizeSpin_->setValue(s.fontSize());

    const auto theme = s.theme();
    for (int i = 0; i < themeCombo_->count(); ++i) {
        if (themeCombo_->itemData(i).toString() == theme) {
            themeCombo_->setCurrentIndex(i);
            break;
        }
    }
    tabWidthSpin_->setValue(s.tabWidth());
    lineNumbersCheck_->setChecked(s.showLineNumbers());
    autoIndentCheck_->setChecked(s.autoIndent());
}

void SettingsDialog::save()
{
    auto &s = hello::core::Settings::instance();
    s.setFontFamily(fontCombo_->currentFont().family());
    s.setFontSize(fontSizeSpin_->value());
    s.setTheme(themeCombo_->currentData().toString());
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

} // namespace hello::ui
