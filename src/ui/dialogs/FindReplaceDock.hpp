// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDockWidget>
#include <QString>

class QCheckBox;
class QLineEdit;
class QPushButton;
class QLabel;

namespace mo::ui {

class CodeEditor;

class FindReplaceDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit FindReplaceDock(QWidget *parent = nullptr);

    QString searchText() const;
    QString replaceText() const;

    void focusSearch();

    void findNext(CodeEditor *editor);
    void findPrev(CodeEditor *editor);
    void replace(CodeEditor *editor);
    void replaceAll(CodeEditor *editor);

signals:
    void searchRequested(const QString &text, bool caseSensitive, bool wholeWords, bool regex);

private:
    void updateStatus(const QString &message);
    CodeEditor *currentEditor() const;

    QLineEdit *searchEdit_ = nullptr;
    QLineEdit *replaceEdit_ = nullptr;
    QCheckBox *caseSensitiveCheck_ = nullptr;
    QCheckBox *wholeWordsCheck_ = nullptr;
    QCheckBox *regexCheck_ = nullptr;
    QPushButton *findNextBtn_ = nullptr;
    QPushButton *findPrevBtn_ = nullptr;
    QPushButton *replaceBtn_ = nullptr;
    QPushButton *replaceAllBtn_ = nullptr;
    QLabel *statusLabel_ = nullptr;
};

} // namespace mo::ui
