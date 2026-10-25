// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QFile>
#include <QLabel>
#include <QPointer>
#include <QStandardPaths>
#include <QTabWidget>
#include <QTemporaryFile>
#include <QTextCursor>
#include <QTextStream>
#include <QtTest>

#include "editor/CodeEditor.hpp"
#include "mainwindow/MainWindow.hpp"

class TestMainWindow : public QObject {
    Q_OBJECT
  private slots:
    void initTestCase();
    void cleanup();
    void testCreateWindow();
    void testOpenFile();
    void testOpenFilesFromCommandLine();
    void testSaveChecksExternalModification();
    void testWordCountStatusBar();

  private:
    static int tabCount(const mo::ui::MainWindow &mw);
    QPointer<mo::ui::MainWindow> m_window;
};

void TestMainWindow::initTestCase() {
    // Offscreen QPA so the suite works in headless CI; test-mode paths keep
    // Settings / SessionManager away from the developer's real config.
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QStandardPaths::setTestModeEnabled(true);
}

void TestMainWindow::cleanup() {
    delete m_window;
    m_window = nullptr;
}

int TestMainWindow::tabCount(const mo::ui::MainWindow &mw) {
    const auto tabs = mw.findChildren<QTabWidget *>();
    if (tabs.isEmpty()) {
        return -1;
    }
    return tabs.first()->count();
}

void TestMainWindow::testCreateWindow() {
    m_window = new mo::ui::MainWindow;
    QVERIFY(m_window != nullptr);
    // The constructor opens one default "Untitled" tab when there is no
    // session to restore.
    QCOMPARE(tabCount(*m_window), 1);
}

void TestMainWindow::testOpenFile() {
    m_window = new mo::ui::MainWindow;
    QCOMPARE(tabCount(*m_window), 1);

    QTemporaryFile tmp;
    QVERIFY(tmp.open());
    {
        QTextStream out(&tmp);
        out << QStringLiteral("integration smoke test\n");
    }
    tmp.close();

    const int before = tabCount(*m_window);
    m_window->openFile(tmp.fileName());
    QCOMPARE(tabCount(*m_window), before + 1);
}

void TestMainWindow::testOpenFilesFromCommandLine() {
    QTemporaryFile tmp1;
    QTemporaryFile tmp2;
    QVERIFY(tmp1.open());
    QVERIFY(tmp2.open());
    tmp1.close();
    tmp2.close();

    // Files given on the command line open in their own tabs, and no extra
    // "Untitled" tab is created.
    m_window = new mo::ui::MainWindow(QStringList{tmp1.fileName(), tmp2.fileName()});
    QCOMPARE(tabCount(*m_window), 2);
}

void TestMainWindow::testSaveChecksExternalModification() {
    QTemporaryFile tmp;
    QVERIFY(tmp.open());
    {
        QTextStream out(&tmp);
        out << QStringLiteral("original\n");
    }
    tmp.close();

    m_window = new mo::ui::MainWindow;
    m_window->openFile(tmp.fileName());
    // The opened file is the current tab (the constructor also opens an
    // "Untitled" tab, so findChildren().first() would be the wrong editor).
    auto *tw = m_window->findChildren<QTabWidget *>().first();
    auto *editor = qobject_cast<mo::ui::CodeEditor *>(tw->currentWidget());
    QVERIFY(editor != nullptr);
    QVERIFY(!editor->isExternallyModified());

    // Our own save must not count as an external modification: the saved
    // state becomes the new baseline.
    m_window->fileSave();
    QVERIFY(!editor->isExternallyModified());

    QFile f(tmp.fileName());
    QVERIFY(f.open(QIODevice::ReadOnly));
    QCOMPARE(QString::fromUtf8(f.readAll()), QStringLiteral("original\n"));
    f.close();

    // A change from outside is detected afterwards (content of a different
    // length so the size alone distinguishes the versions regardless of
    // filesystem timestamp granularity).
    QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
    f.write("externally changed\n");
    f.close();
    QVERIFY(editor->isExternallyModified());

    // Reloading the disk version resets the baseline.
    QVERIFY(editor->loadFile(tmp.fileName()));
    QVERIFY(!editor->isExternallyModified());
    QCOMPARE(editor->toPlainText(), QStringLiteral("externally changed\n"));
}

void TestMainWindow::testWordCountStatusBar() {
    m_window = new mo::ui::MainWindow;
    auto *tw = m_window->findChildren<QTabWidget *>().first();
    auto *editor = qobject_cast<mo::ui::CodeEditor *>(tw->currentWidget());
    QVERIFY(editor != nullptr);

    auto *label = m_window->findChild<QLabel *>(QStringLiteral("wordCountLabel"));
    QVERIFY(label != nullptr);

    // A fresh empty tab shows zero words.
    QCOMPARE(label->text(), QStringLiteral("0 words"));

    // Editing the current document refreshes the full count:
    // 2 CJK characters + 1 latin word = 3.
    editor->setPlainText(QStringLiteral("你好 world"));
    QCOMPARE(label->text(), QStringLiteral("3 words"));

    // Selecting text adds the selected count.
    QTextCursor sel = editor->textCursor();
    sel.setPosition(0);
    sel.setPosition(2, QTextCursor::KeepAnchor); // "你好"
    editor->setTextCursor(sel);
    QCOMPARE(label->text(), QStringLiteral("3 words (2 selected)"));

    // Collapsing the selection removes it again.
    QTextCursor cur = editor->textCursor();
    cur.clearSelection();
    editor->setTextCursor(cur);
    QCOMPARE(label->text(), QStringLiteral("3 words"));
}

QTEST_MAIN(TestMainWindow)
#include "test_mainwindow.moc"
