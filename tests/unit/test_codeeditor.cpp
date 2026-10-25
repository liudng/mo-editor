// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QClipboard>
#include <QGuiApplication>
#include <QPointer>
#include <QStandardPaths>
#include <QTextCursor>
#include <QtTest>

#include "editor/CodeEditor.hpp"
#include "mo/core/Settings.hpp"

class TestCodeEditor : public QObject {
    Q_OBJECT
  private slots:
    void initTestCase();
    void cleanup();

    void testColumnSelectionCopy();
    void testColumnTypeReplacesBlock();
    void testColumnZeroWidthBackspace();
    void testColumnBlockDeleteKeepsSelection();
    void testColumnCutKeepsSelection();
    void testColumnPasteKeepsSelection();
    void testColumnZeroWidthPasteSingleLine();
    void testColumnArrowKeysSteerCursor();
    void testColumnArrowCollapseWidth();
    void testColumnCancel();

    void testTabIndentsSelectedLines();
    void testTabIndentsSingleSelectedLine();
    void testTabReplacesPartialSelection();

  private:
    mo::ui::CodeEditor *makeEditor(const QString &text, int position);
    QPointer<mo::ui::CodeEditor> m_editor;
};

void TestCodeEditor::initTestCase() {
    // Offscreen QPA so the suite works in headless CI; test-mode paths keep
    // Settings away from the developer's real config.
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QStandardPaths::setTestModeEnabled(true);
    // The Tab-indent tests assert on the indent unit.
    mo::core::Settings::instance().setTabWidth(4);
}

void TestCodeEditor::cleanup() {
    delete m_editor;
    m_editor = nullptr;
}

mo::ui::CodeEditor *TestCodeEditor::makeEditor(const QString &text, int position)
{
    m_editor = new mo::ui::CodeEditor;
    m_editor->setPlainText(text);
    QTextCursor cursor = m_editor->textCursor();
    cursor.setPosition(position);
    m_editor->setTextCursor(cursor);
    return m_editor;
}

void TestCodeEditor::testColumnSelectionCopy() {
    auto *editor = makeEditor(QStringLiteral("abcdef\nghijkl\nmnopqr\n"), 1);

    // Alt+Shift+Down x2, Alt+Shift+Right x2 -> rows 0-2, columns [1,3).
    QTest::keyClick(editor, Qt::Key_Down, Qt::AltModifier | Qt::ShiftModifier);
    QTest::keyClick(editor, Qt::Key_Down, Qt::AltModifier | Qt::ShiftModifier);
    QTest::keyClick(editor, Qt::Key_Right, Qt::AltModifier | Qt::ShiftModifier);
    QTest::keyClick(editor, Qt::Key_Right, Qt::AltModifier | Qt::ShiftModifier);
    QVERIFY(editor->hasColumnSelection());

    editor->columnCopy();
    QCOMPARE(QGuiApplication::clipboard()->text(), QStringLiteral("bc\nhi\nno"));
}

void TestCodeEditor::testColumnTypeReplacesBlock() {
    auto *editor = makeEditor(QStringLiteral("abcdef\nghijkl\n"), 1);

    // Two rows, columns [1,3).
    QTest::keyClick(editor, Qt::Key_Down, Qt::AltModifier | Qt::ShiftModifier);
    QTest::keyClick(editor, Qt::Key_Right, Qt::AltModifier | Qt::ShiftModifier);
    QTest::keyClick(editor, Qt::Key_Right, Qt::AltModifier | Qt::ShiftModifier);

    QTest::keyClick(editor, Qt::Key_X);
    QCOMPARE(editor->toPlainText(), QStringLiteral("axdef\ngxjkl\n"));

    // The whole block edit must be a single undo step.
    editor->undo();
    QCOMPARE(editor->toPlainText(), QStringLiteral("abcdef\nghijkl\n"));
}

void TestCodeEditor::testColumnZeroWidthBackspace() {
    auto *editor = makeEditor(QStringLiteral("abcd\nxy\n"), 2);

    // Zero-width column over both rows at column 2.
    QTest::keyClick(editor, Qt::Key_Down, Qt::AltModifier | Qt::ShiftModifier);
    QTest::keyClick(editor, Qt::Key_Backspace);

    QCOMPARE(editor->toPlainText(), QStringLiteral("acd\nx\n"));
    // The column selection persists after a zero-width delete.
    QVERIFY(editor->hasColumnSelection());

    // The caret column follows the deletion: a second Backspace deletes the
    // next character to the left, like a single caret would.
    QTest::keyClick(editor, Qt::Key_Backspace);
    QCOMPARE(editor->toPlainText(), QStringLiteral("cd\n\n"));
    QVERIFY(editor->hasColumnSelection());

    // Typing after the deletes lands where the characters were removed.
    QTest::keyClick(editor, Qt::Key_Z);
    QCOMPARE(editor->toPlainText(), QStringLiteral("zcd\nz\n"));
}

void TestCodeEditor::testColumnBlockDeleteKeepsSelection() {
    auto *editor = makeEditor(QStringLiteral("abcdef\nghijkl\n"), 1);

    // Two rows, columns [1,3).
    QTest::keyClick(editor, Qt::Key_Down, Qt::AltModifier | Qt::ShiftModifier);
    QTest::keyClick(editor, Qt::Key_Right, Qt::AltModifier | Qt::ShiftModifier);
    QTest::keyClick(editor, Qt::Key_Right, Qt::AltModifier | Qt::ShiftModifier);

    QTest::keyClick(editor, Qt::Key_Backspace);
    QCOMPARE(editor->toPlainText(), QStringLiteral("adef\ngjkl\n"));
    // The block delete collapses the rectangle to a zero-width multi-cursor
    // at its left edge instead of leaving column mode.
    QVERIFY(editor->hasColumnSelection());

    // Further typing still applies to every row.
    QTest::keyClick(editor, Qt::Key_X);
    QCOMPARE(editor->toPlainText(), QStringLiteral("axdef\ngxjkl\n"));
}

void TestCodeEditor::testColumnCutKeepsSelection() {
    auto *editor = makeEditor(QStringLiteral("abcdef\nghijkl\n"), 1);

    QTest::keyClick(editor, Qt::Key_Down, Qt::AltModifier | Qt::ShiftModifier);
    QTest::keyClick(editor, Qt::Key_Right, Qt::AltModifier | Qt::ShiftModifier);
    QTest::keyClick(editor, Qt::Key_Right, Qt::AltModifier | Qt::ShiftModifier);

    QTest::keyClick(editor, Qt::Key_X, Qt::ControlModifier);
    QCOMPARE(QGuiApplication::clipboard()->text(), QStringLiteral("bc\nhi"));
    QCOMPARE(editor->toPlainText(), QStringLiteral("adef\ngjkl\n"));
    // Cut keeps the multi-cursor alive, like a block delete.
    QVERIFY(editor->hasColumnSelection());
}

void TestCodeEditor::testColumnPasteKeepsSelection() {
    auto *editor = makeEditor(QStringLiteral("abcdef\nghijkl\n"), 1);

    QTest::keyClick(editor, Qt::Key_Down, Qt::AltModifier | Qt::ShiftModifier);
    QTest::keyClick(editor, Qt::Key_Right, Qt::AltModifier | Qt::ShiftModifier);
    QTest::keyClick(editor, Qt::Key_Right, Qt::AltModifier | Qt::ShiftModifier);

    QTest::keyClick(editor, Qt::Key_C, Qt::ControlModifier);
    QCOMPARE(QGuiApplication::clipboard()->text(), QStringLiteral("bc\nhi"));

    // In-place paste: the rectangle is replaced by identical text, and the
    // selection survives with the caret column advanced past it.
    QTest::keyClick(editor, Qt::Key_V, Qt::ControlModifier);
    QCOMPARE(editor->toPlainText(), QStringLiteral("abcdef\nghijkl\n"));
    QVERIFY(editor->hasColumnSelection());

    QTest::keyClick(editor, Qt::Key_X);
    QCOMPARE(editor->toPlainText(), QStringLiteral("abcxdef\nghixjkl\n"));
}

void TestCodeEditor::testColumnZeroWidthPasteSingleLine() {
    auto *editor = makeEditor(QStringLiteral("abcd\nxy\n"), 2);

    // Zero-width multi-cursor over both rows at column 2.
    QTest::keyClick(editor, Qt::Key_Down, Qt::AltModifier | Qt::ShiftModifier);

    QGuiApplication::clipboard()->setText(QStringLiteral("Z"));
    QTest::keyClick(editor, Qt::Key_V, Qt::ControlModifier);
    // Single-line clipboard text is inserted on every row, like typing.
    QCOMPARE(editor->toPlainText(), QStringLiteral("abZcd\nxyZ\n"));
    QVERIFY(editor->hasColumnSelection());
}

void TestCodeEditor::testColumnArrowKeysSteerCursor() {
    auto *editor = makeEditor(QStringLiteral("aaa\nbbb\nccc\nddd\n"), 5); // row 1, col 1

    // Zero-width multi-cursor over rows 1-3 at column 1; a plain Up shifts
    // the whole group one row up (rows 0-2).
    QTest::keyClick(editor, Qt::Key_Down, Qt::AltModifier | Qt::ShiftModifier);
    QTest::keyClick(editor, Qt::Key_Down, Qt::AltModifier | Qt::ShiftModifier);
    QTest::keyClick(editor, Qt::Key_Up);
    QVERIFY(editor->hasColumnSelection());
    QTest::keyClick(editor, Qt::Key_Z);
    QCOMPARE(editor->toPlainText(), QStringLiteral("azaa\nbzbb\nczcc\nddd\n"));

    // Plain Left/Right steer the column, plain Down shifts rows down.
    editor->setPlainText(QStringLiteral("aaa\nbbb\nccc\nddd\n"));
    editor->cancelColumnSelection();
    QTextCursor reset = editor->textCursor();
    reset.setPosition(5);
    editor->setTextCursor(reset);
    QTest::keyClick(editor, Qt::Key_Down, Qt::AltModifier | Qt::ShiftModifier); // rows 1-2, col 1
    QTest::keyClick(editor, Qt::Key_Left);  // column 0
    QTest::keyClick(editor, Qt::Key_Right); // column 1
    QTest::keyClick(editor, Qt::Key_Down);  // rows 2-3
    QVERIFY(editor->hasColumnSelection());
    QTest::keyClick(editor, Qt::Key_Z);
    QCOMPARE(editor->toPlainText(), QStringLiteral("aaa\nbbb\nczcc\ndzdd\n"));
}

void TestCodeEditor::testColumnArrowCollapseWidth() {
    auto *editor = makeEditor(QStringLiteral("abcdef\nghijkl\n"), 1);

    // Two rows, columns [1,3).
    QTest::keyClick(editor, Qt::Key_Down, Qt::AltModifier | Qt::ShiftModifier);
    QTest::keyClick(editor, Qt::Key_Right, Qt::AltModifier | Qt::ShiftModifier);
    QTest::keyClick(editor, Qt::Key_Right, Qt::AltModifier | Qt::ShiftModifier);

    // Left collapses the rectangle to a zero-width cursor at its left edge;
    // typing then inserts there (the arrows move the caret, not content).
    QTest::keyClick(editor, Qt::Key_Left);
    QVERIFY(editor->hasColumnSelection());
    QTest::keyClick(editor, Qt::Key_Z);
    QCOMPARE(editor->toPlainText(), QStringLiteral("azbcdef\ngzhijkl\n"));

    // Right on a width selection collapses to the right edge instead.
    editor->setPlainText(QStringLiteral("abcdef\nghijkl\n"));
    editor->cancelColumnSelection();
    QTextCursor reset = editor->textCursor();
    reset.setPosition(1);
    editor->setTextCursor(reset);
    QTest::keyClick(editor, Qt::Key_Down, Qt::AltModifier | Qt::ShiftModifier);
    QTest::keyClick(editor, Qt::Key_Right, Qt::AltModifier | Qt::ShiftModifier);
    QTest::keyClick(editor, Qt::Key_Right, Qt::AltModifier | Qt::ShiftModifier);
    QTest::keyClick(editor, Qt::Key_Right); // collapses to the right edge (column 3)
    QVERIFY(editor->hasColumnSelection());
    QTest::keyClick(editor, Qt::Key_Z);
    QCOMPARE(editor->toPlainText(), QStringLiteral("abczdef\nghizjkl\n"));
}

void TestCodeEditor::testColumnCancel() {
    auto *editor = makeEditor(QStringLiteral("abc\ndef\n"), 0);

    // Escape cancels.
    QTest::keyClick(editor, Qt::Key_Down, Qt::AltModifier | Qt::ShiftModifier);
    QVERIFY(editor->hasColumnSelection());
    QTest::keyClick(editor, Qt::Key_Escape);
    QVERIFY(!editor->hasColumnSelection());

    // A mouse click cancels too.
    QTest::keyClick(editor, Qt::Key_Down, Qt::AltModifier | Qt::ShiftModifier);
    QVERIFY(editor->hasColumnSelection());
    QTest::mouseClick(editor->viewport(), Qt::LeftButton);
    QVERIFY(!editor->hasColumnSelection());
}

void TestCodeEditor::testTabIndentsSelectedLines() {
    // Selection from mid line 0 to mid line 2: every touched line indents,
    // the content is not replaced.
    auto *editor = makeEditor(QStringLiteral("aaaa\nbbbb\ncccc\n"), 2);
    QTextCursor sel = editor->textCursor();
    sel.setPosition(11, QTextCursor::KeepAnchor); // line 2, col 1
    editor->setTextCursor(sel);

    QTest::keyClick(editor, Qt::Key_Tab);
    QCOMPARE(editor->toPlainText(), QStringLiteral("    aaaa\n    bbbb\n    cccc\n"));

    // The selection survives on the same characters, shifted with them.
    QCOMPARE(editor->textCursor().selectionStart(), 6);
    QCOMPARE(editor->textCursor().selectionEnd(), 23);

    // One Tab is one undo step.
    editor->undo();
    QCOMPARE(editor->toPlainText(), QStringLiteral("aaaa\nbbbb\ncccc\n"));
}

void TestCodeEditor::testTabIndentsSingleSelectedLine() {
    // Whole line selected without the trailing newline (Home, Shift+End).
    auto *editor = makeEditor(QStringLiteral("hello\nworld\n"), 0);
    QTextCursor sel = editor->textCursor();
    sel.setPosition(5, QTextCursor::KeepAnchor); // "hello"
    editor->setTextCursor(sel);

    QTest::keyClick(editor, Qt::Key_Tab);
    QCOMPARE(editor->toPlainText(), QStringLiteral("    hello\nworld\n"));

    // The selection still covers the whole indented line: Tab indents again
    // instead of overwriting the line.
    QCOMPARE(editor->textCursor().selectionStart(), 0);
    QCOMPARE(editor->textCursor().selectionEnd(), 9);
    QTest::keyClick(editor, Qt::Key_Tab);
    QCOMPARE(editor->toPlainText(), QStringLiteral("        hello\nworld\n"));

    // A selection ending at column 0 (Home, Shift+Down) must not indent the
    // line it only touches at its start.
    editor->setPlainText(QStringLiteral("aaaa\nbbbb\ncccc\n"));
    editor->cancelColumnSelection();
    QTextCursor reset = editor->textCursor();
    reset.setPosition(0);
    reset.setPosition(5, QTextCursor::KeepAnchor); // "aaaa\n"
    editor->setTextCursor(reset);

    QTest::keyClick(editor, Qt::Key_Tab);
    QCOMPARE(editor->toPlainText(), QStringLiteral("    aaaa\nbbbb\ncccc\n"));
}

void TestCodeEditor::testTabReplacesPartialSelection() {
    // A selection inside one line keeps the classic typing-over behaviour.
    auto *editor = makeEditor(QStringLiteral("hello\nworld\n"), 1);
    QTextCursor sel = editor->textCursor();
    sel.setPosition(4, QTextCursor::KeepAnchor); // "ell"
    editor->setTextCursor(sel);

    QTest::keyClick(editor, Qt::Key_Tab);
    QCOMPARE(editor->toPlainText(), QStringLiteral("h    o\nworld\n"));
}

QTEST_MAIN(TestCodeEditor)
#include "test_codeeditor.moc"
