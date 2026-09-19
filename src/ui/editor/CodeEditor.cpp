// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CodeEditor.hpp"

#include "mo/core/Logger.hpp"
#include "mo/core/Settings.hpp"

#include <QClipboard>
#include <QFile>
#include <QFileInfo>
#include <QFont>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QMouseEvent>
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
    const int key = event->key();

    // Column selection: Alt+Shift+arrows start or steer the rectangle.
    if ((event->modifiers() & Qt::AltModifier) && (event->modifiers() & Qt::ShiftModifier)
        && key >= Qt::Key_Left && key <= Qt::Key_Down) {
        columnMoveCorner(key);
        return;
    }

    if (columnActive_) {
        switch (key) {
        case Qt::Key_Escape:
            cancelColumnSelection();
            return;
        case Qt::Key_Backspace:
            columnDelete(false);
            return;
        case Qt::Key_Delete:
            columnDelete(true);
            return;
        case Qt::Key_Tab:
            columnInsertText(
                QString(mo::core::Settings::instance().tabWidth(), QLatin1Char(' ')));
            return;
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
            // Plain arrow keys steer the multi-cursor group without leaving
            // column mode (modified arrows fall through and cancel).
            if (event->modifiers() == Qt::NoModifier) {
                columnMoveCaret(key);
                return;
            }
            break;
        default:
            break;
        }
        if (key == Qt::Key_C || key == Qt::Key_X || key == Qt::Key_V) {
            if ((event->modifiers() & Qt::ControlModifier)) {
                if (key == Qt::Key_C) {
                    columnCopy();
                } else if (key == Qt::Key_X) {
                    columnCut();
                } else {
                    columnPaste();
                }
                return;
            }
        }
        // Plain text input (no Ctrl/Alt/Meta modifiers).
        if (event->text().size() == 1
            && !(event->modifiers()
                 & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier))) {
            columnInsertText(event->text());
            return;
        }
        // Modifier-only presses must not cancel the selection: they arrive
        // as separate key events while composing the next Alt+Shift+Arrow
        // chord (users press and release the modifiers for every key).
        if (key == Qt::Key_Shift || key == Qt::Key_Control || key == Qt::Key_Alt
            || key == Qt::Key_Meta || key == Qt::Key_AltGr) {
            QPlainTextEdit::keyPressEvent(event);
            return;
        }
        // Anything else cancels the column selection and falls through to
        // the regular handling below.
        cancelColumnSelection();
    }

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

void CodeEditor::mousePressEvent(QMouseEvent *event)
{
    cancelColumnSelection();
    QPlainTextEdit::mousePressEvent(event);
}

void CodeEditor::paintEvent(QPaintEvent *event)
{
    QPlainTextEdit::paintEvent(event);

    // A zero-width column selection is drawn as a thin caret on every row.
    if (!columnActive_ || columnRectLeft() != columnRectRight()) {
        return;
    }
    QPainter painter(viewport());
    const QColor color = palette().color(QPalette::Highlight);
    for (int line = columnFirstLine(); line <= columnLastLine(); ++line) {
        const auto block = document()->findBlockByNumber(line);
        if (!block.isValid()) {
            break;
        }
        QTextCursor cursor(document());
        cursor.setPosition(block.position() + qMin(columnRectLeft(), block.text().size()));
        const QRect rect = cursorRect(cursor);
        painter.fillRect(QRect(rect.left(), rect.top(), 2, rect.height()), color);
    }
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

    // Column (rectangular) selection: one highlighted range per row, clamped
    // to each line's length.
    if (columnActive_) {
        const QColor selColor = palette().color(QPalette::Highlight);
        const int left = columnRectLeft();
        const int right = columnRectRight();
        for (int line = columnFirstLine(); line <= columnLastLine(); ++line) {
            const auto block = document()->findBlockByNumber(line);
            if (!block.isValid()) {
                break;
            }
            const int len = block.text().size();
            const int start = qMin(left, len);
            const int end = qMin(right, len);
            if (end <= start) {
                continue;
            }
            QTextEdit::ExtraSelection sel;
            sel.format.setBackground(selColor);
            sel.cursor = QTextCursor(document());
            sel.cursor.setPosition(block.position() + start);
            sel.cursor.setPosition(block.position() + end, QTextCursor::KeepAnchor);
            extraSelections.append(sel);
        }
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
    cancelColumnSelection();
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

// ---- Column (rectangular) selection ----

bool CodeEditor::hasColumnSelection() const
{
    return columnActive_;
}

int CodeEditor::columnRectLeft() const
{
    return qMin(columnAnchor_.column, columnCorner_.column);
}

int CodeEditor::columnRectRight() const
{
    return qMax(columnAnchor_.column, columnCorner_.column);
}

int CodeEditor::columnFirstLine() const
{
    return qMin(columnAnchor_.line, columnCorner_.line);
}

int CodeEditor::columnLastLine() const
{
    return qMax(columnAnchor_.line, columnCorner_.line);
}

void CodeEditor::startColumnSelection()
{
    const auto cursor = textCursor();
    columnAnchor_ = {cursor.blockNumber(), cursor.columnNumber()};
    // Place the active corner one line down so the selection is immediately
    // visible; stay on the current line when there is no line below.
    columnCorner_ = {qMin(columnAnchor_.line + 1, blockCount() - 1),
                     columnAnchor_.column};
    columnActive_ = true;
    columnUpdateCaret();
    highlightCurrentLine();
    viewport()->update();
}

void CodeEditor::cancelColumnSelection()
{
    if (!columnActive_) {
        return;
    }
    columnActive_ = false;
    highlightCurrentLine();
    viewport()->update();
}

void CodeEditor::columnMoveCorner(int key)
{
    if (!columnActive_) {
        const auto cursor = textCursor();
        columnAnchor_ = {cursor.blockNumber(), cursor.columnNumber()};
        columnCorner_ = columnAnchor_;
        columnActive_ = true;
    }
    switch (key) {
    case Qt::Key_Up:
        columnCorner_.line = qMax(0, columnCorner_.line - 1);
        break;
    case Qt::Key_Down:
        columnCorner_.line = qMin(blockCount() - 1, columnCorner_.line + 1);
        break;
    case Qt::Key_Left:
        columnCorner_.column = qMax(0, columnCorner_.column - 1);
        break;
    case Qt::Key_Right:
        ++columnCorner_.column;
        break;
    default:
        return;
    }
    columnUpdateCaret();
    highlightCurrentLine();
    viewport()->update();
}

void CodeEditor::columnMoveCaret(int key)
{
    // Plain arrow keys steer the whole multi-cursor group without leaving
    // column mode: Up/Down shift every row, Left/Right shift the column.
    // A selection with width collapses first, the way a plain caret would
    // jump to the edge of its selection before moving further.
    const int left = columnRectLeft();
    const int right = columnRectRight();
    const bool hadWidth = left != right;

    switch (key) {
    case Qt::Key_Left:
        columnAnchor_.column = columnCorner_.column =
            hadWidth ? left : qMax(left - 1, 0);
        break;
    case Qt::Key_Right:
        columnAnchor_.column = columnCorner_.column = hadWidth ? right : right + 1;
        break;
    case Qt::Key_Up:
    case Qt::Key_Down: {
        if (hadWidth) {
            // The selection collapses to the active corner's column.
            columnAnchor_.column = columnCorner_.column;
        }
        const int first = qMin(columnAnchor_.line, columnCorner_.line);
        const int last = qMax(columnAnchor_.line, columnCorner_.line);
        const int bottom = document()->blockCount() - 1;
        if (key == Qt::Key_Up && first > 0) {
            --columnAnchor_.line;
            --columnCorner_.line;
        } else if (key == Qt::Key_Down && last < bottom) {
            ++columnAnchor_.line;
            ++columnCorner_.line;
        }
        break;
    }
    default:
        return;
    }
    columnUpdateCaret();
    highlightCurrentLine();
    viewport()->update();
}

void CodeEditor::columnUpdateCaret()
{
    const auto block = document()->findBlockByNumber(columnCorner_.line);
    if (!block.isValid()) {
        return;
    }
    QTextCursor cursor(document());
    cursor.setPosition(block.position() + qMin(columnCorner_.column, block.text().size()));
    setTextCursor(cursor);
}

void CodeEditor::columnInsertText(const QString &text)
{
    if (!columnActive_ || text.isEmpty()) {
        return;
    }
    const int left = columnRectLeft();
    const int right = columnRectRight();

    QTextCursor edit(textCursor());
    edit.beginEditBlock();
    for (int line = columnLastLine(); line >= columnFirstLine(); --line) {
        const auto block = document()->findBlockByNumber(line);
        if (!block.isValid()) {
            continue;
        }
        const int len = block.text().size();
        const int start = qMin(left, len);
        QTextCursor cursor(document());
        cursor.setPosition(block.position() + start);
        if (right > left) {
            const int end = qMin(right, len);
            if (end > start) {
                cursor.setPosition(block.position() + end, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
            }
        }
        cursor.insertText(text);
    }
    edit.endEditBlock();

    // Keep the column selection alive after the inserted text.
    columnAnchor_.column = columnCorner_.column = left + text.size();
    columnUpdateCaret();
    highlightCurrentLine();
    viewport()->update();
}

void CodeEditor::columnDelete(bool forward)
{
    if (!columnActive_) {
        return;
    }
    if (columnRectLeft() != columnRectRight()) {
        columnDeleteBlock();
        return;
    }

    const int col = columnRectLeft();
    QTextCursor edit(textCursor());
    edit.beginEditBlock();
    for (int line = columnLastLine(); line >= columnFirstLine(); --line) {
        const auto block = document()->findBlockByNumber(line);
        if (!block.isValid()) {
            continue;
        }
        const int len = block.text().size();
        const int pos = qMin(col, len);
        QTextCursor cursor(document());
        if (forward) {
            if (pos >= len) {
                continue; // nothing to delete after end of line
            }
            cursor.setPosition(block.position() + pos);
            cursor.deleteChar();
        } else {
            if (pos == 0) {
                continue; // nothing to delete before start of line
            }
            cursor.setPosition(block.position() + pos - 1);
            cursor.deleteChar();
        }
    }
    edit.endEditBlock();

    // The zero-width column selection persists, like multi-cursor editing.
    // A backward delete also shifts the caret column left so consecutive
    // Backspaces keep deleting the character left of the caret, matching
    // single-caret behaviour.
    if (!forward && col > 0) {
        columnAnchor_.column = columnCorner_.column = col - 1;
    }
    columnUpdateCaret();
    highlightCurrentLine();
    viewport()->update();
}

void CodeEditor::columnDeleteBlock()
{
    const int left = columnRectLeft();
    const int right = columnRectRight();

    QTextCursor edit(textCursor());
    edit.beginEditBlock();
    for (int line = columnLastLine(); line >= columnFirstLine(); --line) {
        const auto block = document()->findBlockByNumber(line);
        if (!block.isValid()) {
            continue;
        }
        const int len = block.text().size();
        const int start = qMin(left, len);
        const int end = qMin(right, len);
        if (end <= start) {
            continue;
        }
        QTextCursor cursor(document());
        cursor.setPosition(block.position() + start);
        cursor.setPosition(block.position() + end, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
    }
    edit.endEditBlock();

    // Keep the multi-cursor alive: the rectangle collapses to a zero-width
    // column at its left edge, ready for further typing or deletion. Only
    // Esc, a mouse click, or a plain key exits column mode.
    columnAnchor_.column = columnCorner_.column = left;
    columnUpdateCaret();
    highlightCurrentLine();
    viewport()->update();
}

void CodeEditor::columnCopy()
{
    if (!columnActive_ || columnRectLeft() == columnRectRight()) {
        return;
    }
    const int left = columnRectLeft();
    const int right = columnRectRight();
    QString text;
    for (int line = columnFirstLine(); line <= columnLastLine(); ++line) {
        const auto block = document()->findBlockByNumber(line);
        if (!block.isValid()) {
            break;
        }
        const int len = block.text().size();
        const int start = qMin(left, len);
        const int end = qMin(right, len);
        if (!text.isEmpty()) {
            text += QLatin1Char('\n');
        }
        if (end > start) {
            text += block.text().mid(start, end - start);
        }
    }
    QGuiApplication::clipboard()->setText(text);
}

void CodeEditor::columnCut()
{
    if (!columnActive_ || columnRectLeft() == columnRectRight()) {
        return;
    }
    columnCopy();
    columnDeleteBlock();
}

void CodeEditor::columnPaste()
{
    if (!columnActive_) {
        paste();
        return;
    }
    const QStringList lines = QGuiApplication::clipboard()->text().split(QLatin1Char('\n'));
    const int rows = columnLastLine() - columnFirstLine() + 1;
    if (lines.size() == 1 && rows > 1) {
        // Single-line clipboard text acts like multi-cursor typing: insert it
        // on every selected row (replacing the rectangle when it has width).
        columnInsertText(lines.constFirst());
        return;
    }
    if (lines.size() != rows) {
        // Line counts do not match: fall back to a normal paste at the caret.
        cancelColumnSelection();
        paste();
        return;
    }

    const int left = columnRectLeft();
    const int right = columnRectRight();
    QTextCursor edit(textCursor());
    edit.beginEditBlock();
    for (int i = lines.size() - 1; i >= 0; --i) {
        const auto block = document()->findBlockByNumber(columnFirstLine() + i);
        if (!block.isValid()) {
            continue;
        }
        const int len = block.text().size();
        const int start = qMin(left, len);
        QTextCursor cursor(document());
        cursor.setPosition(block.position() + start);
        if (right > left) {
            const int end = qMin(right, len);
            if (end > start) {
                cursor.setPosition(block.position() + end, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
            }
        }
        cursor.insertText(lines.at(i));
    }
    edit.endEditBlock();

    // Keep the column selection alive after the pasted text; the caret
    // column advances past the longest pasted line.
    int width = 0;
    for (const QString &line : lines) {
        width = qMax(width, line.size());
    }
    columnAnchor_.column = columnCorner_.column = left + width;
    columnUpdateCaret();
    highlightCurrentLine();
    viewport()->update();
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
