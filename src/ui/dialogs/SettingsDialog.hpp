// SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDialog>

class QCheckBox;
class QComboBox;
class QFontComboBox;
class QSpinBox;

namespace hello::ui {

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

signals:
    void settingsChanged();

private slots:
    void onAccept();

private:
    void load();
    void save();

    QFontComboBox *fontCombo_ = nullptr;
    QSpinBox *fontSizeSpin_ = nullptr;
    QComboBox *themeCombo_ = nullptr;
    QSpinBox *tabWidthSpin_ = nullptr;
    QCheckBox *lineNumbersCheck_ = nullptr;
    QCheckBox *autoIndentCheck_ = nullptr;
};

} // namespace hello::ui
