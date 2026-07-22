// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QPlainTextEdit>
#include <QString>

namespace KSyntaxHighlighting {
class Repository;
class SyntaxHighlighter;
class Theme;
}

namespace mo::ui {

class LineNumberArea;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);
    ~CodeEditor() override;

    QString filePath() const;

    bool loadFile(const QString &path);
    bool save();
    bool saveAs(const QString &path);

    void setLanguage(const QString &extension);
    void findNext(const QString &text);

    // Re-applies the KSyntaxHighlighting color theme that best matches the
    // current widget palette. Call after switching between light/dark themes.
    void applyHighlightingTheme();

    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    friend class LineNumberArea;
    void lineNumberAreaPaintEvent(QPaintEvent *event);

    LineNumberArea *lineNumberArea_ = nullptr;
    QString filePath_;
    KSyntaxHighlighting::SyntaxHighlighter *highlighter_ = nullptr;
    static KSyntaxHighlighting::Repository *repository_;
};

} // namespace mo::ui
