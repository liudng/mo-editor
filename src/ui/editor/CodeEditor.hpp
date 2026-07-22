// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QPlainTextEdit>
#include <QString>

#include <KSyntaxHighlighting/Theme>

namespace KSyntaxHighlighting {
class Repository;
class SyntaxHighlighter;
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
    QString tabTitle() const;

    bool loadFile(const QString &path);
    bool save();
    bool saveAs(const QString &path);

    void setLanguage(const QString &extension);
    void findNext(const QString &text);
    void findPrev(const QString &text);

    // Re-applies the KSyntaxHighlighting color theme that best matches the
    // current widget palette. Call after switching between light/dark themes.
    void applyHighlightingTheme();

    // Reads font, tab width, line-numbers, and auto-indent from Settings and
    // applies them to this editor.
    void applyEditorSettings();

    void setShowLineNumbers(bool show);
    void zoomFont(int delta);

    int lineNumberAreaWidth();

signals:
    void modificationChanged(bool modified);
    void titleChanged(const QString &title);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);
    void onModificationChanged();

private:
    friend class LineNumberArea;
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    void updateTabTitle();

    LineNumberArea *lineNumberArea_ = nullptr;
    QString filePath_;
    bool showLineNumbers_ = true;
    KSyntaxHighlighting::SyntaxHighlighter *highlighter_ = nullptr;
    KSyntaxHighlighting::Theme highlightingTheme_;
    static KSyntaxHighlighting::Repository *repository_;
};

} // namespace mo::ui
