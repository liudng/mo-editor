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

    // ---- Column (rectangular) selection ----
    // A pseudo-selection spanning the same character range on consecutive
    // lines; started with Alt+Shift+Arrows or startColumnSelection(). Plain
    // arrows steer the multi-cursor group; Esc / mouse click / other plain
    // keys cancel it.
    bool hasColumnSelection() const;
    void startColumnSelection();
    void cancelColumnSelection();
    // Clipboard operations aware of the active column selection.
    void columnCopy();
    void columnCut();
    void columnPaste();

signals:
    void modificationChanged(bool modified);
    void titleChanged(const QString &title);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);
    void onModificationChanged();

private:
    friend class LineNumberArea;
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    void updateTabTitle();

    struct ColumnPoint {
        int line = 0;
        int column = 0;
    };
    int columnRectLeft() const;
    int columnRectRight() const;
    int columnFirstLine() const;
    int columnLastLine() const;
    void columnMoveCorner(int key);
    void columnMoveCaret(int key);
    void columnUpdateCaret();
    void columnInsertText(const QString &text);
    void columnDelete(bool forward);
    void columnDeleteBlock();

    LineNumberArea *lineNumberArea_ = nullptr;
    QString filePath_;
    bool showLineNumbers_ = true;
    KSyntaxHighlighting::SyntaxHighlighter *highlighter_ = nullptr;
    KSyntaxHighlighting::Theme highlightingTheme_;
    static KSyntaxHighlighting::Repository *repository_;

    bool columnActive_ = false;
    ColumnPoint columnAnchor_;
    ColumnPoint columnCorner_;
};

} // namespace mo::ui
