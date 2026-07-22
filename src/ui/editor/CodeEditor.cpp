// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CodeEditor.hpp"

#include "mo/core/Logger.hpp"

#include <QFile>
#include <QFileInfo>
#include <QPaintEvent>
#include <QPainter>
#include <QPalette>
#include <QResizeEvent>
#include <QSaveFile>
#include <QStringConverter>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextStream>

#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/Repository>
#include <KSyntaxHighlighting/SyntaxHighlighter>
#include <KSyntaxHighlighting/Theme>

namespace mo::ui {

KSyntaxHighlighting::Repository *CodeEditor::repository_ = nullptr;

class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor *editor) : QWidget(editor), editor_(editor) {}

    QSize sizeHint() const override
    {
        return {editor_->lineNumberAreaWidth(), 0};
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        editor_->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *editor_;
};

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
{
    if (!repository_) {
        repository_ = new KSyntaxHighlighting::Repository();
    }
    highlighter_ = new KSyntaxHighlighting::SyntaxHighlighter(document());
    applyHighlightingTheme();

    lineNumberArea_ = new LineNumberArea(this);

    connect(this, &QPlainTextEdit::blockCountChanged,
            this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest,
            this, &CodeEditor::updateLineNumberArea);
    connect(this, &QPlainTextEdit::cursorPositionChanged,
            this, &CodeEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

CodeEditor::~CodeEditor() = default;

QString CodeEditor::filePath() const { return filePath_; }

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    const int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int newBlockCount)
{
    Q_UNUSED(newBlockCount);
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy) {
        lineNumberArea_->scroll(0, dy);
    } else {
        lineNumberArea_->update(0, rect.y(), lineNumberArea_->width(), rect.height());
    }
    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    const QRect cr = contentsRect();
    lineNumberArea_->setGeometry(
        QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        QColor lineColor;
        if (highlightingTheme_.isValid()) {
            lineColor = QColor::fromRgba(highlightingTheme_.editorColor(
                KSyntaxHighlighting::Theme::EditorColorRole::CurrentLine));
        }
        if (!lineColor.isValid()) {
            lineColor = palette().color(QPalette::AlternateBase);
        }
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }
    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea_);

    // Prefer the KSyntaxHighlighting theme's line-number colors so the gutter
    // tracks the same dark/light scheme as the syntax highlighting. Fall back
    // to palette roles when no theme is loaded yet.
    QColor bgColor;
    QColor fgColor;
    if (highlightingTheme_.isValid()) {
        bgColor = QColor::fromRgba(highlightingTheme_.editorColor(
            KSyntaxHighlighting::Theme::EditorColorRole::IconBorder));
        fgColor = QColor::fromRgba(highlightingTheme_.editorColor(
            KSyntaxHighlighting::Theme::EditorColorRole::LineNumbers));
    }
    if (!bgColor.isValid()) {
        bgColor = palette().color(QPalette::Window);
    }
    if (!fgColor.isValid()) {
        // BrightText is light on dark palettes and dark on light palettes,
        // giving reasonable contrast for line numbers in both themes.
        fgColor = palette().color(QPalette::BrightText);
        if (!fgColor.isValid()) {
            fgColor = palette().color(QPalette::Text);
        }
    }

    painter.fillRect(event->rect(), bgColor);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    painter.setPen(fgColor);
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            const QString number = QString::number(blockNumber + 1);
            painter.drawText(0, top, lineNumberArea_->width() - 2,
                             fontMetrics().height(), Qt::AlignRight, number);
        }
        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

bool CodeEditor::loadFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        mo::core::Logger::warning("Failed to open: " + path);
        return false;
    }
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    setPlainText(in.readAll());
    file.close();

    filePath_ = path;
    setLanguage(QFileInfo(path).suffix());
    document()->setModified(false);
    return true;
}

bool CodeEditor::save()
{
    if (filePath_.isEmpty()) {
        return false;
    }
    return saveAs(filePath_);
}

bool CodeEditor::saveAs(const QString &path)
{
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << toPlainText();
    out.flush();
    if (file.commit()) {
        filePath_ = path;
        document()->setModified(false);
        return true;
    }
    return false;
}

void CodeEditor::setLanguage(const QString &extension)
{
    if (!repository_ || extension.isEmpty()) {
        return;
    }
    const auto def = repository_->definitionForFileName(QStringLiteral("file.") + extension);
    if (def.isValid()) {
        highlighter_->setDefinition(def);
    }
}

void CodeEditor::applyHighlightingTheme()
{
    if (!repository_ || !highlighter_) {
        return;
    }
    // themeForPalette() picks a light or dark KSyntaxHighlighting theme that
    // matches the current widget palette, so the syntax colors stay readable
    // when the application switches between light and dark QSS stylesheets.
    const auto theme = repository_->themeForPalette(palette());
    if (theme.isValid()) {
        highlighter_->setTheme(theme);
        highlightingTheme_ = theme;
    }
    // Repaint the line-number area and current-line highlight so their colors
    // track the new theme instead of keeping stale palette-based colors.
    if (lineNumberArea_) {
        lineNumberArea_->update();
    }
    highlightCurrentLine();
}

void CodeEditor::findNext(const QString &text)
{
    if (text.isEmpty()) {
        return;
    }
    QTextCursor cursor = textCursor();
    cursor = document()->find(text, cursor);
    if (cursor.isNull()) {
        cursor = document()->find(text);
    }
    if (!cursor.isNull()) {
        setTextCursor(cursor);
    }
}

} // namespace mo::ui
