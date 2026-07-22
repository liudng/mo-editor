// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CodeEditor.hpp"

#include "mo/core/Logger.hpp"
#include "mo/core/Settings.hpp"

#include <QFile>
#include <QFileInfo>
#include <QFont>
#include <QKeyEvent>
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
    connect(document(), &QTextDocument::modificationChanged,
            this, &CodeEditor::onModificationChanged);

    applyEditorSettings();
    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

CodeEditor::~CodeEditor() = default;

QString CodeEditor::filePath() const { return filePath_; }

QString CodeEditor::tabTitle() const
{
    QString title;
    if (filePath_.isEmpty()) {
        title = tr("Untitled");
    } else {
        title = QFileInfo(filePath_).fileName();
    }
    if (document()->isModified()) {
        title.prepend(QStringLiteral("* "));
    }
    return title;
}

int CodeEditor::lineNumberAreaWidth()
{
    if (!showLineNumbers_) {
        return 0;
    }
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

void CodeEditor::keyPressEvent(QKeyEvent *event)
{
    // Auto-indent: when pressing Enter, copy leading whitespace from current line.
    if (mo::core::Settings::instance().autoIndent()
        && event->key() == Qt::Key_Return && !(event->modifiers() & Qt::ShiftModifier)) {
        auto cursor = textCursor();
        const QString lineText = cursor.block().text();
        QString indent;
        for (const auto ch : lineText) {
            if (ch == QLatin1Char(' ') || ch == QLatin1Char('\t')) {
                indent += ch;
            } else {
                break;
            }
        }
        QPlainTextEdit::keyPressEvent(event);
        if (!indent.isEmpty()) {
            cursor = textCursor();
            cursor.insertText(indent);
        }
        return;
    }

    // Tab key: insert spaces according to tabWidth setting.
    if (event->key() == Qt::Key_Tab && !(event->modifiers() & Qt::ControlModifier)) {
        const int tw = mo::core::Settings::instance().tabWidth();
        insertPlainText(QString(tw, QLatin1Char(' ')));
        return;
    }

    QPlainTextEdit::keyPressEvent(event);
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
    updateTabTitle();
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
        updateTabTitle();
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
    const auto theme = repository_->themeForPalette(palette());
    if (theme.isValid()) {
        highlighter_->setTheme(theme);
        highlightingTheme_ = theme;
    }
    if (lineNumberArea_) {
        lineNumberArea_->update();
    }
    highlightCurrentLine();
}

void CodeEditor::applyEditorSettings()
{
    auto &s = mo::core::Settings::instance();
    QFont font(s.fontFamily(), s.fontSize());
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    setFont(font);

    setShowLineNumbers(s.showLineNumbers());
}

void CodeEditor::setShowLineNumbers(bool show)
{
    showLineNumbers_ = show;
    if (lineNumberArea_) {
        lineNumberArea_->setVisible(show);
    }
    updateLineNumberAreaWidth(0);
    if (lineNumberArea_) {
        const QRect cr = contentsRect();
        lineNumberArea_->setGeometry(
            QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
    }
}

void CodeEditor::zoomFont(int delta)
{
    QFont f = font();
    const int newSize = f.pointSize() + delta;
    if (newSize < 6 || newSize > 48) {
        return;
    }
    f.setPointSize(newSize);
    setFont(f);
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

void CodeEditor::findPrev(const QString &text)
{
    if (text.isEmpty()) {
        return;
    }
    QTextCursor cursor = textCursor();
    cursor = document()->find(text, cursor, QTextDocument::FindBackward);
    if (cursor.isNull()) {
        cursor = document()->find(text, QTextCursor(), QTextDocument::FindBackward);
    }
    if (!cursor.isNull()) {
        setTextCursor(cursor);
    }
}

void CodeEditor::onModificationChanged()
{
    updateTabTitle();
    emit modificationChanged(document()->isModified());
}

void CodeEditor::updateTabTitle()
{
    emit titleChanged(tabTitle());
}

} // namespace mo::ui
