// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QPointer>
#include <QStandardPaths>
#include <QTabWidget>
#include <QTemporaryFile>
#include <QTextStream>
#include <QtTest>

#include "mainwindow/MainWindow.hpp"

class TestMainWindow : public QObject {
    Q_OBJECT
  private slots:
    void initTestCase();
    void cleanup();
    void testCreateWindow();
    void testOpenFile();
    void testOpenFilesFromCommandLine();

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

QTEST_MAIN(TestMainWindow)
#include "test_mainwindow.moc"
