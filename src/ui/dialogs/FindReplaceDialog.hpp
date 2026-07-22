// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDialog>
#include <QString>

class QCheckBox;
class QLineEdit;

namespace mo::ui {

class FindReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindReplaceDialog(QWidget *parent = nullptr);

    QString searchText() const;
    QString replaceText() const;
    bool caseSensitive() const;
    bool wholeWords() const;
    bool useRegex() const;

public slots:
    void find();
    void replace();
    void replaceAll();

signals:
    void findRequested(const QString &text, bool caseSensitive, bool wholeWords, bool regex);
    void replaceRequested(const QString &find, const QString &replace,
                          bool caseSensitive, bool wholeWords, bool regex);
    void replaceAllRequested(const QString &find, const QString &replace,
                             bool caseSensitive, bool wholeWords, bool regex);

private:
    QLineEdit *searchEdit_ = nullptr;
    QLineEdit *replaceEdit_ = nullptr;
    QCheckBox *caseSensitiveCheck_ = nullptr;
    QCheckBox *wholeWordsCheck_ = nullptr;
    QCheckBox *regexCheck_ = nullptr;
};

} // namespace mo::ui
