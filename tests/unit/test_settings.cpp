// SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QCoreApplication>
#include <QStandardPaths>
#include <QtTest>

#include "hello/core/Settings.hpp"

class TestSettings : public QObject {
    Q_OBJECT
  private slots:
    void initTestCase();
    void testDefaultValues();
    void testLoadSaveRoundtrip();
    void testRecentFilesList();
};

void TestSettings::initTestCase() {
    // Redirect QStandardPaths locations to a sandbox so the singleton's
    // settings file does not clobber the developer's real config.
    QStandardPaths::setTestModeEnabled(true);
    auto &settings = hello::core::Settings::instance();
    settings.setRecentFiles({});
    settings.save();
}

void TestSettings::testDefaultValues() {
    auto &settings = hello::core::Settings::instance();
    QCOMPARE(settings.fontFamily(), QStringLiteral("Monospace"));
    QCOMPARE(settings.fontSize(), 11);
    QCOMPARE(settings.theme(), QStringLiteral("System"));
    QCOMPARE(settings.tabWidth(), 4);
    QVERIFY(settings.showLineNumbers());
    QVERIFY(settings.autoIndent());
}

void TestSettings::testLoadSaveRoundtrip() {
    auto &settings = hello::core::Settings::instance();

    settings.setFontFamily(QStringLiteral("Noto Sans Mono"));
    settings.setFontSize(14);
    settings.setTheme(QStringLiteral("Dark"));
    settings.setTabWidth(2);
    settings.setShowLineNumbers(false);
    settings.setAutoIndent(false);
    settings.save();

    // Mutate in-memory state, then reload from disk and verify persistence.
    settings.setFontFamily(QStringLiteral("temporary"));
    settings.setFontSize(99);
    settings.setTheme(QStringLiteral("Light"));
    settings.load();

    QCOMPARE(settings.fontFamily(), QStringLiteral("Noto Sans Mono"));
    QCOMPARE(settings.fontSize(), 14);
    QCOMPARE(settings.theme(), QStringLiteral("Dark"));
    QCOMPARE(settings.tabWidth(), 2);
    QVERIFY(!settings.showLineNumbers());
    QVERIFY(!settings.autoIndent());
}

void TestSettings::testRecentFilesList() {
    auto &settings = hello::core::Settings::instance();
    settings.setRecentFiles({});
    QVERIFY(settings.recentFiles().isEmpty());

    settings.addRecentFile(QStringLiteral("/tmp/file1.txt"));
    settings.addRecentFile(QStringLiteral("/tmp/file2.txt"));
    // Re-adding promotes to the front and removes the duplicate.
    settings.addRecentFile(QStringLiteral("/tmp/file1.txt"));

    const auto recent = settings.recentFiles();
    QCOMPARE(recent.size(), 2);
    QCOMPARE(recent.first(), QStringLiteral("/tmp/file1.txt"));
    QCOMPARE(recent.last(), QStringLiteral("/tmp/file2.txt"));

    // The recent-files list is capped at 10 entries.
    settings.setRecentFiles({});
    for (int i = 0; i < 15; ++i) {
        settings.addRecentFile(QStringLiteral("/tmp/f%1.txt").arg(i));
    }
    QCOMPARE(settings.recentFiles().size(), 10);
    QCOMPARE(settings.recentFiles().first(), QStringLiteral("/tmp/f14.txt"));
}

QTEST_GUILESS_MAIN(TestSettings)
#include "test_settings.moc"
